#include "publicdata.h"


using namespace PublicData;

AlDiagTaskProcessInfo::AlDiagTaskProcessInfo()
{
    runState = Stop;
    result = "";
}

//QDataStream&  operator >> (QDataStream& in, AlDiagTaskProcessInfo& data)
//{
//    in >>  data.id >>  data.nodeID >>  data.diagsisID >>  data.channTreeID
//        >>  data.runState >>  data.result >>  data.taskSerialnumber >>  data.jobtaskID
//        >>  data.cchannID >>  data.treeID;
//    return in;
//}

//QDataStream&  operator << (QDataStream& out, AlDiagTaskProcessInfo& data)
//{
//    out << data.id << data.nodeID << data.diagsisID << data.channTreeID
//        << data.runState << data.result << data.taskSerialnumber << data.jobtaskID
//        << data.cchannID << data.treeID;
//    return out;
//}

QDebug operator<<(QDebug debug, const PublicData::Device& dev)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "PublicData::Device("
                    << " id:" << dev.id << ','
                    << " name:" << dev.name << ','
                    << " alias:" << dev.alias << ','
                    << " stationId:" << dev.stationId << ','
                    << " subSystemId:" << dev.subSystemId << ','
                    << " deviceType:" << dev.deviceType << ','
                    << " deviceTypeName:" << dev.deviceTypeName << ','
                    << " dataTemplateId:" << dev.dataTemplateId << ','
                    << " receivePortId:" << dev.receivePortId << ','
                    << " receiveIP:" << dev.receiveIP << ','
                    << " remoteIP:" << dev.remoteIP << ','
                    << " remotePortId:" << dev.remotePortId << ','
                    << " protocolId:" << dev.protocolId << ','
                    << " commType:" << dev.commType << ','
                    << " address:" << dev.address << ','
                    << " queryTime:" << dev.queryTime << ','
                    << " comment1:" << dev.comment1 << ')';

    return debug;
}
