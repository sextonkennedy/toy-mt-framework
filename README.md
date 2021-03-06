# Skeleton Framework for Emulating CMS Applications
This project is a simple emulation of CMS production jobs using different task based parallelism technologies.  It captures the complexity of CMS workflows without having to use the ~4million lines of code that would otherwise involve.  Back in 2012 when this was first written this emulation was used to select TBB as the basis of a CMS' multi-threaded framemework.  This framework is now available as an open source project here:
<URL for stitched>

## COMPILING

1) You will need to modify the Makefile and Makefile_single to point to your copy of the needed third party libraries
2) Then just compile
	make 
	make -f Makefile_single
3) Alternatively you can use cmake <figure out how to use Jim's cmake infrastructure>
% Note libdispach is deprecated, so this information is now obsolete: If you are on linux, you'll need to get and install a version of libdispatch
%        https://www.heily.com/trac/libdispatch
%which need libkqueue
%        http://mark.heily.com/book/export/html/52
	
You will wind up with four executables

- BusyWaitCalibrate: 
Used to calibrate how long it takes to run a simple integration function. This is used for tests where you want a module to actually emulate doing work for the time specified in the data file (as apposed to just sleeping for that time).  The output of this executable which should be run on each machine being benchmarked, is input to the *Demo applications.  See below for instructions.

- OpenMPProccessingDemo and TBBProcessingDemo:
Processe configuration files to run trivial modules in a way analogous to a full HEP framework. Useful for testing scalability of the system.

- SingleThreadedDemo:
Similar to *Demo applications except it only uses one thread and is implemented in the most trivial (and fastest) way possible. Used for timing comparisons with the same configurations passed to *Demo.

## TESTING
To see that the program works at all, you can run *Demo using the simple test configuration

./*Demo *ProcessingDemo/test_config.json

This should generate an output like (From a Core 2 Duo MacBookPro)

demo::PassFilter
demo::PassFilter
demo::PassFilter
demo::ThreadSaferGetterFilter
demo::BusyWaitProducer
demo::BusyWaitProducer
# simultaneous events:4 total # events:3000 cpu time:90.5964 real time:47.41 events/sec:63.2778

--------------------------------------
## CALIBRATING

In order to simulate the effect of actually doing HEP type work, the code does a simple integration to eat up 100% of a CPU. However, how long a given loop takes needs to be calibrated for the machine being used.  This is done by running 'BusyWaitCalibrate'

Running this program gives an output similar to the following

./BusyWaitCalibrate 
43.3487
100000 0.004535 2.20507e+07
195.367
1000000 0.042997 2.32574e+07
199.928
10000000 0.441726 2.26385e+07
6.4598
100000000 5.10827 1.95761e+07

The grouping of numbers is the results from running 4 different iteration values, each differing by a factor of 10. The lone number on a line can be ignored. The the numbers on one line are
	- the number of iterations used
	- the length of time it took to do that many iterations
	- the ratio of (# of iterations)/(length of time)
From experimentation, I've found that the first 3 iterations tend to be converging to a reasonable common number but the longest iteration tends to be slower, probably because the operating system is taking some time from the job in order to do some other work.  Therefore I tend to take the largest value of the ratio from the first three values.

The ratio is then used to set the value in the various configuration files. The value is
	process.options.busyWaitScaleFactor
The easy way to change the value is to just search for 'busyWaitScaleFactor' in the configuration file since it only appears once.


--------------------------------------
## CONFIGURATIONS

The configuration files are JSON format files which contain the information such as
- What is the busy wait scale factor to use (process.options.busyWaitScaleFactor)
- How many events should the job 'process' (process.source.iterations)
- How many events should be processed simultaneously (process.options.nSimultaneousEvents)
- What 'modules' to run and how the various 'modules' are related

The demo framework uses standard CMS concepts:
	-Tasks are encapsulated in 'modules' where 'modules' only communicate by adding object to and reading objects from the Event
	-Modules which only read data from the Event and decide if we should continue processing an Event are called Filters
	-Filters are placed in sequence into a Path
	-The job allows multiple Paths, where each Path is considered independent and can be run in parallel
	-Modules which add data to an event are called Producers. Producers can depend on data in the Event. A Producer will be run the first time a data item is requested from a given Event.
	-Data in the event is identified by the 'label' which was assigned to the Producer which makes the data.

The list of Filters is defined in the 'process.filters' value.
The list of Producers is defined in the 'process.producers' value.
The list of Paths is defined in the 'process.paths' value.

Filters and Producers are configured with the mandatory parameters
	'@label': the label used to uniquely defined the module (and the data the module makes)
	'@type':  what C++ class to instantiate when creating the module
Each C++ class type can then have its own parameters, although most accept
	'eventTimes': how long the module runs for a given event. Specifying time for each event allows one to measure effects of correlations of times between modules. The units are seconds.
	'threadType': used by *Demo to decide how thread safe to treat the module. The allowed values are
		ThreadSafeBetweenInstances: the same instance of a module can be passed multiple events simultaneously
		ThreadSafeBetweenModules: the same instance of a module can only handle one event at a time but it does not interact with any other module
		ThreadUnsafe: the module can not be run at the same time as other 'ThreadUnsafe' modules
	'toGet': A list of the data to acquire from the event. The list is pair of 'label' (which is the label of the Producer) and a 'product' which is not used now.

## EXPERIMENTS

The experimental configurations are found in cms-data:
reco_minbias_busywait.config: 
This simulates doing event Reconstruction on a common and quick to process event type where each module is using real CPU time. The job has one Filter which is simulating the behavior of the output module which would write a ROOT file. This Filter is set to be 'ThreadUnsafe' to simulate present ROOT behavior. The source is set to be ThreadSafeBetweenModules so only one request is processed at a time in the module. All other modules are configured with ThreadSafeBetweenInstances so are assumed to be perfectly thread safe.

reco_minbias_sleeping.config
Similar to reco_minbias_busywait.config but instead of having modules use real CPU time, they 'sleep' instead. The libdispatch is smart enough to see when a thread is sleeping and then assign more work to the system. This configuration allows one to push the nSimultaneousEvents to much larger levels than the actual number of CPUs on the machine. I was able to run 700 nSimultaneousEvents on a 2 core machine.

reco_minbias_sleeping_1000.config
The same as reco_minbias_sleeping.config, however instead of each module having specified timing for 100 events, this has timing for 1000 events. This is only here for historic reasons.

reco_minbias_sleeping_betweenmodules.config
The same as reco_minbias_sleeping.config except the Producers have been set to only be able to process one event at a time (ThreadSafeBetweenModules). This allows scaling test for what happens if modules are not fully thread safe.

reco_minbias_sleeping_perfectIO.config
The same as reco_minbias_sleeping.config except both the source and output (i.e. the Filter) are set to completely thread safe (ThreadSafeBetweenInstances). This tests the scaling limits of the framework itself.

reco_multijet_sleeping.config
The same as reco_minbias_sleeping.config except the events take much longer to process.

reco_multijet_sleeping_1000.config
The same as reco_multijet_sleeping.config exception each module specifies 1000 event times rather than just 100. This is only here for historic reasons.

So the suggested way to proceed with a experimenting is
1) Run reco_minbias_busywait.config while changing both 'nSimultaneousEvents' and 'iterations' (trying to keep the total job time about constant) until you see the events/sec results plateau
2) Run reco_minbias_sleeping.config while changing both 'nSimultaneousEvents' and 'iterations' (trying to keep the total job time about constant) until you see the events/sec results plateau. You should be able to go to much higher nSimultaneousEvents. Also, results from 1) with the same 'nSimultaneousEvents' should be the same as 2)
3) Same as 2) but using reco_minbias_sleeping_betweenmodules.config
4) Same as 2) but using reco_minbias_sleeping_perfectIO.config

