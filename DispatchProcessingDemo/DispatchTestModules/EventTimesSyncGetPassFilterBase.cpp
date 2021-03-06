//
//  EventTimesSyncGetPassFilterBase.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/13/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <unistd.h>

#include "EventTimesSyncGetPassFilterBase.h"
#include "thread_type_from_config.h"
#include "Event.h"

static const std::string s_blank("");
namespace demo {
  
  EventTimesSyncGetPassFilterBase::EventTimesSyncGetPassFilterBase(const boost::property_tree::ptree& iConfig):
  ConfiguredFilter(iConfig,thread_type_from_config(iConfig)){
    
    for(const auto& v: iConfig.get_child("toGet")) {
      m_toGet.push_back(v.second.get<std::string>("label"));
    }
    m_eventTimes.reserve(iConfig.get_child("eventTimes").size());
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("eventTimes")) {
      m_eventTimes.push_back(v.second.get_value<float>());
    }
  }
  
  bool 
  EventTimesSyncGetPassFilterBase::filter(const edm::Event& iEvent) {
    //printf("Producer %s\n",label().c_str());
    int sum=0;
    for(const auto& get: m_toGet){
      sum +=iEvent.get(get,s_blank);
      //printf("%s got %s with value %i\n",label().c_str(), (*it)->label().c_str(), iEvent.get((*it)));
    }
    unsigned long index = iEvent.index() % m_eventTimes.size();
    
    wait(m_eventTimes[index]);
    return true;
  }
}
