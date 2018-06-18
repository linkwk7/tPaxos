#include "pch.h"
#include "test_config.h"
#include "paxos/group.h"

TEST(GroupTest, Basic) {
	Config<tPaxos::Group> conf(3);

	for (int i = 0; i < 10; i++) {
		conf.oGroups[0]->NewPropose(tPaxos::ToString(i));
	}
}

void ParallelPropose(Config<tPaxos::Group> & conf, int Node, int Num) {
	std::vector<std::thread> threads;

	for (int i = 0; i < Num; i++) {
		threads.push_back(
			std::thread(
				[&](int index) {conf.oGroups[Node]->NewPropose(tPaxos::ToString(index)); }, i
			)
		);
	}

	for (int i = 0; i < Num; i++) {
		threads[i].join();
	}
}

TEST(GroupTest, ConcurrentSinglePoint) {
	Config<tPaxos::Group> conf(3);

	ParallelPropose(conf, 0, 100000);
}

TEST(GroupTest, ConcurrentMultiPoint) {
	Config<tPaxos::Group> conf(3);

	std::vector<std::thread> threads;
	for (int i = 0; i < 100000; i++) {
		threads.push_back(
			std::thread(
				[&](int index) {conf.oGroups[index%2]->NewPropose(tPaxos::ToString(index)); }, i
			)
		);
	}

	for (int i = 0; i < 100000; i++) {
		threads[i].join();
	}
}

TEST(GroupTest, ConcurrentSinglePointUnreliable) {
	Config<tPaxos::Group> conf(3);
	conf.oTransfer.SetDropRate(30);
	conf.oTransfer.SetReplicateRate(10);

	ParallelPropose(conf, 0, 1000);

	// Wait for node sync
	std::this_thread::sleep_for(std::chrono::seconds(20));
}

TEST(GroupTest, ConcurrentMultiPointUnreliable) {
	Config<tPaxos::Group> conf(3);
	conf.oTransfer.SetDropRate(30);
	conf.oTransfer.SetReplicateRate(10);

	std::vector<std::thread> threads;
	for (int i = 0; i < 1000; i++) {
		threads.push_back(
			std::thread(
				[&](int index) {conf.oGroups[index % 2]->NewPropose(tPaxos::ToString(index)); }, i
			)
		);
	}

	for (int i = 0; i < 1000; i++) {
		threads[i].join();
	}

	// Wait for node sync
	std::this_thread::sleep_for(std::chrono::seconds(20));
}

TEST(GroupTest, DisconnectMinority) {
	Config<tPaxos::Group> conf(3);
	
	ParallelPropose(conf, 0, 200);

	conf.oTransfer.Disconnect(1);

	ParallelPropose(conf, 0, 200);

	conf.oTransfer.Connect(1);

	ParallelPropose(conf, 0, 200);
}

TEST(GroupTest, DisconnectMajority) {
	// Because the time use to retransmit is growing expontial, i will rewrite this 
	// test after rewrite interface provide by group

	Config<tPaxos::Group> conf(3);

	ParallelPropose(conf, 1, 200);

	std::vector<std::thread> threads;
	for (int i = 0; i < 1000; i++) {
		threads.push_back(
			std::thread(
				[&](int index) {conf.oGroups[0]->NewPropose(tPaxos::ToString(index)); }, i
			)
		);
	}

	conf.oTransfer.Disconnect(1);
	conf.oTransfer.Disconnect(2);

	std::this_thread::sleep_for(std::chrono::seconds(5));

	conf.oTransfer.Connect(1);
	conf.oTransfer.Connect(2);

	for (int i = 0; i < 1000; i++) {
		threads[i].join();
	}
}