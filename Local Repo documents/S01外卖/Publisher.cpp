#include "S01_SupermarketTypeSupportImpl.h"
#include <dds\DdsDcpsInfrastructureC.h>
#include <dds\DdsDcpsPublicationC.h>
#include <dds\DCPS\StaticIncludes.h>
#include <dds\DCPS\Service_Participant.h>
#include <dds\DCPS\Marked_Default_Qos.h>
//#include <dds\DCPS\PublisherImpl.h>
#include <dds\DCPS\WaitSet.h>
#include <ace\streams.h>
//#include <ace\OS_NS_unistd.h>
#include <ace\Log_Msg.h>
#include <windows.h>
#include<time.h>
#include <sys/timeb.h>
using namespace std;

int main(int argc, char *argv[]) {
	srand((unsigned)time(NULL));
	double waste = rand() / (RAND_MAX + 1.0);
	try {

		//注入点

		//初始化域参数工厂
		DDS::DomainParticipantFactory_var dpf =
			TheParticipantFactoryWithArgs(argc, argv);
		//创建域参数
		DDS::DomainParticipant_var participant =
			dpf->create_participant(42, // domain ID
				PARTICIPANT_QOS_DEFAULT,
				0,  // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//域参数创建异常处理
		if (!participant) {
			std::cerr << "create_participant failed." << std::endl;
			return 1;
		}
		//注册S01_Supermarket::DefaultInf类型的类型支持类
		S01_Supermarket::DefaultInfTypeSupport_var ts = new S01_Supermarket::DefaultInfTypeSupportImpl;
		//注册异常处理
		if (DDS::RETCODE_OK != ts->register_type(participant, "")) {
			std::cerr << "register_type failed." << std::endl;
			return 1;
		}


		//=============================================================================================-
		//创建主题（DefaultInf S01_Supermarket）
		CORBA::String_var default_name = ts->get_type_name();
		DDS::Topic_var default_topic =
			participant->create_topic("DefaultInf S01_Supermarket",
				default_name,
				TOPIC_QOS_DEFAULT,
				0,   // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//主题创建异常处理
		if (!default_topic) {
			std::cerr << "create_topic failed." << std::endl;
			return 1;
		}
		//创建主题（UpdateInf S01_Supermarket）
		CORBA::String_var update_name = ts->get_type_name();
		DDS::Topic_var update_topic =
			participant->create_topic("UpdateInf S01_Supermarket",
				update_name,
				TOPIC_QOS_DEFAULT,
				0,   // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//主题创建异常处理
		if (!update_topic) {
			std::cerr << "create_topic failed." << std::endl;
			return 1;
		}		
		//=============================================================================================-


		//创建发布者
		DDS::Publisher_var pub =
			participant->create_publisher(PUBLISHER_QOS_DEFAULT,
				0,    // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//发布者创建异常处理
		if (!pub) {
			std::cerr << "create_publisher failed." << std::endl;
			return 1;
		}

		//=============================================================================================-
		//创建数据写者
		DDS::DataWriter_var DefaultWriter =
			pub->create_datawriter(default_topic,
				DATAWRITER_QOS_DEFAULT,
				0,    // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//数据写者创建异常
		if (!DefaultWriter) {
			std::cerr << "create_dataDefaultWriter failed." << std::endl;
			return 1;
		}
		//创建数据写者二号
		DDS::DataWriter_var UpdateWriter =
			pub->create_datawriter(update_topic,
				DATAWRITER_QOS_DEFAULT,
				0,    // No listener required
				OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		//数据写者创建异常二号
		if (!UpdateWriter) {
			std::cerr << "create_dataDefaultWriter failed." << std::endl;
			return 1;
		}
		//=============================================================================================-


		//类型窄化
		S01_Supermarket::DefaultInfDataWriter_var UpdateInf_DefaultWriter = S01_Supermarket::DefaultInfDataWriter::_narrow(UpdateWriter);
		S01_Supermarket::DefaultInfDataWriter_var DefaultInf_DefaultWriter = S01_Supermarket::DefaultInfDataWriter::_narrow(DefaultWriter);
		//等待集设定
		/*DDS::StatusCondition_var condition = DefaultWriter->get_statuscondition();
		condition->set_enabled_statuses(DDS::PUBLICATION_MATCHED_STATUS);
		DDS::WaitSet_var ws = new DDS::WaitSet;
		ws->attach_condition(condition);
		while (true)
		{
			DDS::PublicationMatchedStatus matches;
			if (DefaultWriter->get_publication_matched_status(matches) != ::DDS::RETCODE_OK)
			{
				std::cerr << "get_publication_matched_status failed." << std::endl;
				return 1;
			}
			//等待集中有实例
			if (matches.current_count >= 1)
			{
				break;
			}
			DDS::ConditionSeq conditions;
			//等待时间为60秒
			DDS::Duration_t timeout = { 60, 0 };
			if (ws->wait(conditions, timeout) != DDS::RETCODE_OK)
			{
				std::cerr << "wait failed" << std::endl;
				return 1;
			}
		}
		ws->detach_condition(condition);*/
		//写入数据样本
		S01_Supermarket::DefaultInf DefaultInf;
		S01_Supermarket::DefaultInf DefaultInf2;
		DefaultInf.DefaultInfID= 0;
		DefaultInf.DefaultInfMessage = "您有新的饿了么外卖订单，请及时处理！";
		DefaultInf.DefaultInfTopic= "饿了么";
		DefaultInf.PublisherName = "饿了么（我才不饿）的发布者";
		DefaultInf2.DefaultInfID = 0;
		DefaultInf2.DefaultInfMessage = "您有新的美团外卖订单，请及时处理！";
		DefaultInf2.DefaultInfTopic = "美团";
		DefaultInf2.PublisherName = "美团外卖送啥都快";
		//开始发布了,发布default
		//开始发布了，发布update
		DDS::Duration_t timeout = { 30, 0 };
		for (int i = 0; i < 1000; ++i) {
			time_t nowTime;
			time(&nowTime);
			struct timeb tb;
			ftime(&tb);	
			double rand_num = rand() / (RAND_MAX + 1.0);
			if (rand_num>0.5)
			{
				DefaultInf.ClockTime = clock();
				CORBA::String_var s_Time = CORBA::string_dup(ctime(&nowTime));
				DefaultInf.MTime = s_Time;
				DDS::ReturnCode_t error = DefaultInf_DefaultWriter->write(DefaultInf, DDS::HANDLE_NIL);
				++DefaultInf.DefaultInfID;
				if (error != DDS::RETCODE_OK) {
					std::cerr << "default write failed." << std::endl;
					return 1;
				}	
				if (DefaultWriter->wait_for_acknowledgments(timeout) != DDS::RETCODE_OK)
				{
					std::cerr << "wait_for_acknowledgments failed." << std::endl;
					return 1;
				}
				cout << "[S01]发送了一条饿了么主题消息，发送时间是" << ctime(&nowTime) << endl;
			}
			else
			{
				DefaultInf2.ClockTime = clock();
				CORBA::String_var s_Time = CORBA::string_dup(ctime(&nowTime));
				DefaultInf2.MTime = s_Time;
				DDS::ReturnCode_t error = UpdateInf_DefaultWriter->write(DefaultInf2, DDS::HANDLE_NIL);
				++DefaultInf2.DefaultInfID;
				if (error != DDS::RETCODE_OK) {
					std::cerr << "update write failed." << std::endl;
					return 1;
				}
				if (UpdateWriter->wait_for_acknowledgments(timeout) != DDS::RETCODE_OK)
				{
					std::cerr << "wait_for_acknowledgments failed." << std::endl;
					return 1;
				}
				cout << "[S01]发送了一条美团主题消息，发送时间是" << ctime(&nowTime) << endl;
			}
			Sleep(2500);
		}

		//实例清理
		participant->delete_contained_entities();
		dpf->delete_participant(participant);
		TheServiceParticipant->shutdown();
	}
	catch (CORBA::Exception& e) {
		cerr << "Exception caught in main.cpp:" << endl
			<< e << endl;
		ACE_OS::exit(1);
	}
	return 0;
}