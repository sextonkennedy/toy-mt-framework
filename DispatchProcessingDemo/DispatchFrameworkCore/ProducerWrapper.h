//
//  ProducerWrapper.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ProducerWrapper_h
#define DispatchProcessingDemo_ProducerWrapper_h

#include <boost/shared_ptr.hpp>
#include <dispatch/dispatch.h>
#include "GroupHolder.h"
#include "ModuleWrapper.h"
#include "PrefetchAndWorkWrapper.h"

namespace demo {
  
  class Producer;
  class Event;
  
  class ProducerWrapper : private ModuleWrapper,PrefetchAndWorkWrapper {
  public:
    explicit ProducerWrapper(Producer*, Event*);
    ProducerWrapper(const ProducerWrapper&, Event*);
    ProducerWrapper(const ProducerWrapper&);
    ~ProducerWrapper();
    
    //returns the dispatch group assigned to when produce has finished
    // this way callers can either wait on that group or can associate
    // a callback to that group
    GroupHolder doProduceAsync();

    void reset();
    
  private:
    static void do_produceAsyncImpl_task(void*);
    void doProduceAsyncImpl();
    void doWork();
    Producer* producer() const;

    ProducerWrapper& operator=(const ProducerWrapper&);
    
    boost::shared_ptr<Producer> m_producer;
    GroupHolder m_group;
    bool m_wasRun;

  };
}

#endif
