## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_program('footballer-network-sim', ['core', 'network', 'internet', 'point-to-point', 'applications', 'netanim', 'mobility', 'flow-monitor'])
    obj.source = 'footballer-network-sim.cc'
