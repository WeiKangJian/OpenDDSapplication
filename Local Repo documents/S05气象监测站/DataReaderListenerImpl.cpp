#include "DataReaderListenerImpl.h"
#include "YH03TypeSupportC.h"
#include "YH03TypeSupportImpl.h"
#include <dds/DCPS/Service_Participant.h>
#include <ace/streams.h>
#include<time.h>
#include <sys/timeb.h>

void DataReaderListenerImpl::on_data_available(DDS::DataReader_ptr reader)
{
	try {
		YH03::ObserveDataReader_var reader_i = YH03::ObserveDataReader::_narrow(reader);
		if (!reader_i) {
			std::cerr << "read: _narrow failed." << std::endl;
			return;
		}
		YH03::Observe YH03_Observe;
		DDS::SampleInfo si;
		DDS::ReturnCode_t status = reader_i->take_next_sample(YH03_Observe, si);
		if (status == DDS::RETCODE_OK) {
			
			std::cout << "接收到观测站数据" << std::endl;
			//std::cout << "SampleInfo.sample_rank = " << si.sample_rank << std::endl;
			//std::cout << "SampleInfo.instance_state = " << si.instance_state << std::endl;
			if (si.valid_data)
			{
				time_t nowTime;
				time(&nowTime);
				struct timeb tb;
				ftime(&tb);
				std::cout << std::endl << "[S05]发送者消息编号：" << YH03_Observe.YH03_Observe_ID << std::endl
					<< "[S05]发送者毫秒级计时：" << YH03_Observe.ClockTime << std::endl;
				cout << "[S05]发送者主机名：" << YH03_Observe.YH03_Observe_name << std::endl;
				cout << "==================================================" << std::endl;
				std::cout << "  观测站数据编号 = " << YH03_Observe.YH03_Observe_ID << std::endl
					<< "   观测站名称：" << YH03_Observe.YH03_Observe_name << std::endl
					<< "   观测PM2.5：" << YH03_Observe.YH03_Observe_pm2 << std::endl
					<< "   观测温度：" << YH03_Observe.Yh03_Observe_tem << std::endl;
				if (YH03_Observe.YH03_Observe_pm2 > 500)
				{
					cout << "+++++++++++++++++++++++" << endl;
					cout << "PM2.5已经超过500阈值！" << endl;
					cout << "PM2.5已经超过500阈值！" << endl;
					cout << "PM2.5已经超过500阈值！" << endl;
					cout << "+++++++++++++++++++++++" << endl;
				}
				if (YH03_Observe.Yh03_Observe_tem>35)
				{
					cout << "+++++++++++++++++++++++" << endl;
					cout << "高温蓝色预警！" << endl;
					cout << "高温蓝色预警！" << endl;
					cout << "高温蓝色预警！" << endl;
					cout << "+++++++++++++++++++++++" << endl;
				}
				else if (YH03_Observe.Yh03_Observe_tem<-6)
				{
					cout << "+++++++++++++++++++++++" << endl;
					cout << "低温寒冷蓝色预警！" << endl;
					cout << "低温寒冷蓝色预警！" << endl;
					cout << "低温寒冷蓝色预警！" << endl;
					cout << "+++++++++++++++++++++++" << endl;
				}
				cout << "==================================================" << std::endl;
				cout << "[S05]发送者发送时间：" << YH03_Observe.LocalTime;
				cout << "[S05]本地接收时间：" << ctime(&nowTime);
				cout << "[S05]本地消息接收毫秒级计时：" << clock() << std::endl;
				if (YH03_Observe.YH03_Observe_pm2>500 || YH03_Observe.Yh03_Observe_tem>35 || YH03_Observe.Yh03_Observe_tem<-6)
				{
					outf << endl;
					outf << "  观测站名称 = " << YH03_Observe.YH03_Observe_name << endl;
					outf << "  观测站数据编号 = " << YH03_Observe.YH03_Observe_ID << endl;
					outf << "  观测PM2.5：" << YH03_Observe.YH03_Observe_pm2 << endl;
					outf << "   观测温度：" << YH03_Observe.Yh03_Observe_tem << endl;
					if (YH03_Observe.YH03_Observe_pm2 > 500)
					{
						outf << "PM2.5已经超过500阈值！" << endl;
					}
					if (YH03_Observe.Yh03_Observe_tem > 35)
					{
						outf << "高温蓝色预警！" << endl;
					}
					else if (YH03_Observe.Yh03_Observe_tem < -6)
					{
						outf << "低温寒冷蓝色预警！" << endl;
					}
				}

			}
			else if (si.instance_state == DDS::NOT_ALIVE_DISPOSED_INSTANCE_STATE)
			{
				std::cout << "当前实例句柄" << std::endl;
			}
			else if (si.instance_state == DDS::NOT_ALIVE_NO_WRITERS_INSTANCE_STATE)
			{
				std::cout << "instance is unregistered" << std::endl;
			}
			else
			{
				std::cerr << "ERROR: received unknown instance state "
					<< si.instance_state << std::endl;
			}
		}
		else if (status == DDS::RETCODE_NO_DATA) {
			std::cerr << "ERROR: reader received DDS::RETCODE_NO_DATA!" << std::endl;
		}
		else {
			std::cerr << "ERROR: read Observe: Error: " << status << std::endl;
		}
	}
	catch (CORBA::Exception& e) {
		std::cerr << "Exception caught in main.cpp:" << std::endl
			<< e << std::endl;
		ACE_OS::exit(1);
	}

}

void DataReaderListenerImpl::on_requested_deadline_missed(
	DDS::DataReader_ptr,
	const DDS::RequestedDeadlineMissedStatus &)
{
	std::cerr << "DataReaderListenerImpl::on_requested_deadline_missed" << std::endl;
}

void DataReaderListenerImpl::on_requested_incompatible_qos(
	DDS::DataReader_ptr,
	const DDS::RequestedIncompatibleQosStatus &)
{
	std::cerr << "DataReaderListenerImpl::on_requested_incompatible_qos" << std::endl;
}

void DataReaderListenerImpl::on_liveliness_changed(
	DDS::DataReader_ptr,
	const DDS::LivelinessChangedStatus &)
{
	std::cout << "接收者生成或者死亡" << std::endl;
}

void DataReaderListenerImpl::on_subscription_matched(
	DDS::DataReader_ptr,
	const DDS::SubscriptionMatchedStatus &)
{
	std::cout << "匹配成功或匹配结束" << std::endl;
}

void DataReaderListenerImpl::on_sample_rejected(
	DDS::DataReader_ptr,
	const DDS::SampleRejectedStatus&)
{
	std::cerr << "DataReaderListenerImpl::on_sample_rejected" << std::endl;
}

void DataReaderListenerImpl::on_sample_lost(
	DDS::DataReader_ptr,
	const DDS::SampleLostStatus&)
{
	std::cerr << "DataReaderListenerImpl::on_sample_lost" << std::endl;
}