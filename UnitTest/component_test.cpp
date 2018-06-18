#include "pch.h"

#include <thread>
#include <chrono>

#include "gtest/gtest.h"

#include "util/network.h"
#include "util/util.h"
#include "util/timer.h"
#include "util/message.pb.h"
#include "paxos/context.h"
#include "paxos/ioloop.h"

class tReceiver : public tPaxos::NetworkReceiver {
public:
	tReceiver(const uint32_t uID) : uNodeID(uID) {}

	void OnReceiveNewMessage(const std::string & msg) override {
		tPaxos::Dprintf(tPaxos::UtilLevel, std::to_string(uNodeID) + ":" + msg + "\n");
	}

	uint32_t uNodeID;
};

void TestQueueBasic(int index, tPaxos::QueueNetwork * network) {
	for (int i = 0; i < 5; i++) {
		network->Send((index + 1) % 3, tPaxos::ToString("From:", index, ", To:", (index + 1) % 3, " Content:", i));
	}
}

TEST(ComponentTest, QueueTest) {
	tPaxos::QueueTransfer transfer(3);

	std::map<uint32_t, tPaxos::NodeInfo> infoMap;
	for (int i = 0; i < 3; i++) {
		infoMap[i] = tPaxos::NodeInfo(i, "", 0);
	}

	std::vector<tPaxos::QueueNetwork> networks;
	std::vector<tReceiver> receivers;
	std::map<uint32_t, tPaxos::NetworkClient*> netMap;
	// Use three loops to avoid memory reallocation
	for (int i = 0; i < 3; i++) {
		receivers.emplace_back(tReceiver(i));
	}
	for (int i = 0; i < 3; i++) {
		networks.emplace_back(tPaxos::QueueNetwork(i, &transfer, &receivers[i]));
		networks[i].Init(infoMap);
	}
	for (int i = 0; i < 3; i++) {
		netMap[i] = &networks[i];
	}

	transfer.Init(netMap);
	transfer.Run();

	std::vector<std::thread> threads;
	for (int i = 0; i < 3; i++) {
		threads.push_back(std::thread(TestQueueBasic, i, &networks[i]));
	}

	for (int i = 0; i < 3; i++) {
		threads[i].join();
	}
}

TEST(ComponentTest, ContextTest) {
	tPaxos::CommitCtx ctx("1");

	std::thread receiver([&]() {
		ctx.Wait(); 
	});

	std::thread sender([&]() {
		ctx.Commit("1"); 
	});

	sender.join();
	receiver.join();
}

TEST(ComponentTest, TimerTest) {
	uint64_t start = tPaxos::Timers::GetSteadyClockMS();
	
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	tPaxos::Dprintf(tPaxos::UtilLevel, "%lu\n", tPaxos::Timers::GetSteadyClockMS() - start);
}

class GIOLReceiver : public tPaxos::GroupIOLoopReceiver {
public:
	GIOLReceiver(const uint32_t uID) : uAccept(0), uRID(uID) {}

	void Init(tPaxos::GroupIOLoop * pIO) {
		pIOLoop = pIO;
	}

	bool IOLoopOnMessage(tPaxos::PaxosMsg * pMsg) override {
		if (pMsg->instanceid() == uAccept) {
			tPaxos::Dprintf(tPaxos::UtilLevel, "%d[Accept]%d\n", uRID, uAccept);

			uAccept++;

			return true;
		}
		else if (pMsg->instanceid() > uAccept) {
			tPaxos::Dprintf(tPaxos::UtilLevel, "%d[Retry]%d\n", uRID, pMsg->instanceid());

			pIOLoop->AddRetry(pMsg);

			return false;
		}
		else {
			tPaxos::Dprintf(tPaxos::UtilLevel, "Old message %d\n", pMsg->instanceid());
			return true;
		}
	}

	void IOLoopNewPropose() override {
	}

	tPaxos::GroupIOLoop * pIOLoop;
	uint32_t uAccept;
	uint32_t uRID;
};

TEST(ComponentTest, GroupIOLoopTest) {
	GIOLReceiver receiver(0);

	tPaxos::GroupIOLoop ioloop (&receiver);

	receiver.Init(&ioloop);
	ioloop.Run();

	std::vector<tPaxos::PaxosMsg> messages;
	for (int i = 0; i < 6; i++) {
		messages.push_back(tPaxos::PaxosMsg());
		messages[i].set_instanceid(i);
	}

	ioloop.AddMessage(&messages[5]);
	ioloop.AddMessage(&messages[0]);
	ioloop.AddMessage(&messages[3]);
	ioloop.AddMessage(&messages[2]);
	ioloop.AddMessage(&messages[1]);
	ioloop.AddMessage(&messages[4]);

	std::this_thread::sleep_for(std::chrono::seconds(1));

	ioloop.Stop();
}

class IIOLReceiver : public tPaxos::InstanceIOLoopReceiver {
public:
	
	void IOLoopOnTimeout(const uint32_t uTimerID, const int iTimerType) override {
		tPaxos::Dprintf(tPaxos::UtilLevel, "Timer fired : %d with %d at %lu\n", uTimerID, iTimerType, tPaxos::Timers::GetSteadyClockMS()-uStartTime);
	}

	void IOLoopOnMessage(tPaxos::PaxosMsg * pMsg) override {
		tPaxos::Dprintf(tPaxos::UtilLevel, "Receive Message : %d\n", pMsg->instanceid());
	}

	uint64_t uStartTime;
};

TEST(ComponentTest, InstanceIOLoopTest) {
	IIOLReceiver receiver;

	tPaxos::InstanceIOLoop ioloop (&receiver);

	ioloop.Run();

	std::vector<tPaxos::PaxosMsg> messages;
	for (int i = 0; i < 6; i++) {
		messages.push_back(tPaxos::PaxosMsg());
		messages[i].set_instanceid(i);
	}

	receiver.uStartTime = tPaxos::Timers::GetSteadyClockMS();

	for (int i = 0; i < 3; i++) {
		uint32_t ID;
		ioloop.AddTimer(500 * i, i, ID);
	}

	for (int i = 0; i < 6; i++) {
		ioloop.SetAcceptInstanceID(i);
		ioloop.AddMessage(&messages[i]);
	}

	std::this_thread::sleep_for(std::chrono::seconds(2));

	ioloop.Stop();
}

class SIOReceiver : public tPaxos::SyncerIOLoopReceiver {
public:

	void IOLoopOnTimeout() override {
		tPaxos::Dprintf(tPaxos::UtilLevel, "Receiver timeout\n");
	}

	void IOLoopOnTask(const tPaxos::SyncerTask & task) override {
		tPaxos::Dprintf(tPaxos::UtilLevel, "Receive task %u %u\n", task.uNodeID, task.uInstanceID);
	}
};

TEST(ComponentTest, SyncerIOLoopTest) {
	SIOReceiver receiver;
	
	tPaxos::SyncerIOLoop ioloop(&receiver);

	ioloop.Run();

	for (int i = 0; i < 10; i++) {
		tPaxos::SyncerTask task;
		task.uInstanceID = i;
		task.uNodeID = i;

		ioloop.AddTask(task);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(2010));

	ioloop.Stop();
}