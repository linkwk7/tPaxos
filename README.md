##tPaxos
This is an simple implementation of multi-paxos.

There are few issues i haven't fix yet, like: 

* When multiple instance commit paralled, it may degenerate to basic paxos, i known the reason of this question, but i haven't figure out a good solution for this
* Tests need to rewrite to check if consensus result it match commit value, currently i only read log to find out the test result