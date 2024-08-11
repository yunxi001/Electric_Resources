#include "system_config_data.h"

_system_wifi_data *system_wifi_data;

_system_wifi_data* wifi_list_create(char * name,char *password,char *auto_connect)
{
	_system_wifi_data* NewNode = (_system_wifi_data*)heap_caps_malloc(sizeof(_system_wifi_data), MALLOC_CAP_SPIRAM);

	memset(NewNode,0x00,sizeof(_system_wifi_data));
	memcpy(NewNode->name,name,strlen(name));
	memcpy(NewNode->password,password,strlen(password));
	memcpy(NewNode->auto_connect,auto_connect,strlen(auto_connect));

	return NewNode;
}

//链表尾部插入一个节点
void wifi_list_add_node_back(_system_wifi_data **pphead,char * name,char *password,char *auto_connect)
{
	system_config_data_debug("链表尾部插入一个节点");
	_system_wifi_data * tail = *pphead;
	if(tail == NULL)
	{
		system_config_data_debug("创建新节点失败2");
		return;
	}


	_system_wifi_data * NewNode = wifi_list_create(name,password,auto_connect);
	if(NewNode == NULL)
	{
		system_config_data_debug("创建新节点失败");
		return;
	}

	if (*pphead == NULL)
	{
		*pphead = NewNode;
		system_config_data_debug("链表已经初始化");

	}
	else
	{
		system_config_data_debug("开始插入");

		while (tail->next != NULL)
		{
			tail=tail->next;
		}
		tail->next = NewNode;
	}
}
//链表头部插入一个节点
void wifi_list_add_node_front(_system_wifi_data **pphead,char * name,char *password,char *auto_connect)
{
	_system_wifi_data * NewNode = wifi_list_create(name,password,auto_connect);
	NewNode->next = *pphead;
	*pphead = NewNode;
}

//尾节点删除
void wifi_list_delete_node_back(_system_wifi_data **pphead)
{
	//链表为空
	if (*pphead == NULL)
	{
		return;
	}
	else if ((*pphead)->next == NULL)//只有一个节点
	{
		heap_caps_free(*pphead);
		*pphead = NULL;
	}
	else//有多个节点
	{
		_system_wifi_data *prev = NULL;
		_system_wifi_data *tail = *pphead;
		while (tail->next != NULL)
		{
			prev = tail;
			tail = tail->next;
		}
		heap_caps_free(tail);
		prev->next = NULL;
	}
}
//头节点删除
void wifi_list_delete_node_front(_system_wifi_data **pphead)
{
	if (*pphead == NULL)
	{
		return;
	}
	_system_wifi_data *next = (*pphead)->next;
	heap_caps_free(*pphead);
	*pphead = next;
}

//根据名字节点删除
void wifi_list_delete_node_name(_system_wifi_data **pphead,char * name)
{
	return NULL;
}

//查找值为x的节点并返回节点的指针
_system_wifi_data * wifi_list_find_name(_system_wifi_data *phead, char * name)
{
	_system_wifi_data *current = phead;
	while (current != NULL)
	{
		if(strstr(current->name,name))
		{
			return current;
		}
		current = current->next;
	}
	return NULL;
}

uint8_t system_get_wifi_data(void)
{
	const char *config_fime_path = "/spiffs/system/config.json";
	uint32_t file_size=0;

	file_size = system_get_file_size(config_fime_path);
    FILE* file = fopen(config_fime_path, "r");
    if (file == NULL)
	{
        system_config_data_debug("打开配置文件失败");
        return;
    }
	system_config_data_debug("打开配置文件成功");

    char *buf = (char*)heap_caps_malloc(file_size, MALLOC_CAP_SPIRAM);
	memset(buf,0x00,file_size);
    fread(buf, 1, file_size, file);

    fclose(file);
	system_config_data_debug("读出文件:%s\r\n",buf);

	cJSON *json_A = cJSON_Parse(buf);
	cJSON *j_wifi = cJSON_GetObjectItem(json_A,"wifi");

	if( NULL != j_wifi )
	{
		cJSON *j_wifi_item  = j_wifi->child;
		//while( j_wifi_item != NULL )//暂时只读取第一个wifi数据
		{
			char * name   = cJSON_GetObjectItem( j_wifi_item , "name")->valuestring ;
			char * password =  cJSON_GetObjectItem( j_wifi_item , "password")->valuestring ;
			char * aotuconnect =  cJSON_GetObjectItem( j_wifi_item , "auto connect")->valuestring ;
			system_config_data_debug("name:%s password:%s aotuconnect:%s\n",name,password,aotuconnect);
			//wifi_list_add_node_back(&system_wifi_data,name,password,aotuconnect);

			sprintf(system_data.wifi_name,name);
			sprintf(system_data.wifi_password,password);

			j_wifi_item = j_wifi_item->next;
		}
	}

	cJSON_Delete(json_A);
	heap_caps_free(buf);
	return 0;
}


uint8_t system_read_config(void)
{
	const char *config_fime_path = "/spiffs/system/config.json";
	uint32_t file_size=0;

	file_size = system_get_file_size(config_fime_path);
    FILE* file = fopen(config_fime_path, "r");
    if (file == NULL)
	{
        system_config_data_debug("打开配置文件失败");
        return;
    }
	system_config_data_debug("打开配置文件成功");

    char *buf = (char*)heap_caps_malloc(file_size, MALLOC_CAP_SPIRAM);
	memset(buf,0x00,file_size);
    fread(buf, 1, file_size, file);

    fclose(file);
	system_config_data_debug("读出文件:%s\r\n",buf);

	cJSON *json_A = cJSON_Parse(buf);
	cJSON *j_wifi = cJSON_GetObjectItem(json_A,"wifi");
	cJSON *j_system_set = cJSON_GetObjectItem(json_A,"system set");
	cJSON *j_clock_data = cJSON_GetObjectItem(json_A,"clock data");


	//----system data ----//
	if( NULL != j_system_set )
	{
		cJSON *j_system_set_item  = j_system_set->child;
		system_data.backlight = cJSON_GetObjectItem( j_system_set_item , "backlight")->valueint;
		system_data.Language = cJSON_GetObjectItem( j_system_set_item , "language")->valueint;
		system_data.wifi_kaiguan = cJSON_GetObjectItem( j_system_set_item , "wifi switch")->valueint;
		char * str_bilibili_id =  cJSON_GetObjectItem( j_system_set_item , "bilibili id")->valuestring ;

		sprintf(system_data.bilibili_Data.id,"%s",str_bilibili_id);

		system_config_data_debug("背光:%d 语言:%d WiFi开关:%d",system_data.backlight,system_data.Language,system_data.wifi_kaiguan);
	}
	//----system data ----//

	//----clock data ----//
	if( NULL != j_clock_data )
	{
		cJSON *j_clock_data_item  = j_clock_data->child;
		system_data.clock_data.type = cJSON_GetObjectItem( j_clock_data_item , "type")->valueint;
		system_data.clock_data.style = cJSON_GetObjectItem( j_clock_data_item , "style")->valueint;

		system_config_data_debug("时钟类型:%d 样式:%d",system_data.clock_data.type,system_data.clock_data.style);
	}
	//----clock data ----//

	//----wifi ----//
	if( NULL != j_wifi )
	{
		cJSON *j_wifi_item  = j_wifi->child;
		//while( j_wifi_item != NULL )//暂时只读取第一个wifi数据
		{
			char * name   = cJSON_GetObjectItem( j_wifi_item , "name")->valuestring ;
			char * password =  cJSON_GetObjectItem( j_wifi_item , "password")->valuestring ;
			char * aotuconnect =  cJSON_GetObjectItem( j_wifi_item , "auto connect")->valuestring ;
			system_config_data_debug("name:%s password:%s aotuconnect:%s",name,password,aotuconnect);
			//wifi_list_add_node_back(&system_wifi_data,name,password,aotuconnect);

			sprintf(system_data.wifi_name,name);
			sprintf(system_data.wifi_password,password);

			j_wifi_item = j_wifi_item->next;
		}
	}
	//----wifi ----//

	cJSON_Delete(json_A);
	heap_caps_free(buf);
	return 0;
}



int system_save_config(void)
{

	const char *config_fime_path = "/spiffs/system/config.json";
	uint32_t file_size=0;

	cJSON* root = NULL;

	root = cJSON_CreateObject();

	//----system set ----//
	cJSON* arr_system_set = cJSON_CreateArray();
	cJSON* obj_system_set = cJSON_CreateObject();

	cJSON_AddNumberToObject(obj_system_set, "wifi switch",system_get_wifi_kaiguan());
	cJSON_AddNumberToObject(obj_system_set , "backlight", system_data.backlight);
	cJSON_AddNumberToObject(obj_system_set , "language", system_get_Language());
	cJSON_AddStringToObject(obj_system_set , "bilibili id", system_data.bilibili_Data.id);

	cJSON_AddItemToArray(arr_system_set, obj_system_set);
	cJSON_AddItemToObject(root, "system set", arr_system_set);
	//----system set ----//


	//----clock data ----//
	cJSON* arr_clock_data = cJSON_CreateArray();
	cJSON* obj_clock_data = cJSON_CreateObject();
	cJSON_AddNumberToObject(obj_clock_data, "type",system_data.clock_data.type);
	cJSON_AddNumberToObject(obj_clock_data , "style", system_data.clock_data.style);

	cJSON_AddItemToArray(arr_clock_data, obj_clock_data);
	cJSON_AddItemToObject(root, "clock data", arr_clock_data);
	//----clock data ----//

	//----wifi ----//
	cJSON* arr_wifi= cJSON_CreateArray();
	cJSON* obj_wifi = cJSON_CreateObject();
	cJSON_AddStringToObject(obj_wifi, "name",system_data.wifi_name);
	cJSON_AddStringToObject(obj_wifi, "password",system_data.wifi_password);
	cJSON_AddStringToObject(obj_wifi, "auto connect","true");

	cJSON_AddItemToArray(arr_wifi, obj_wifi);
	cJSON_AddItemToObject(root, "wifi", arr_wifi);
	//----wifi ----//

	char* str = cJSON_PrintUnformatted(root);
	int len = strlen(str);
	system_config_data_debug("len:%d %s\n",len, cJSON_Print(root));
	system_config_data_debug("保存文件");
	file_size = system_get_file_size(config_fime_path);
    FILE* file = fopen(config_fime_path, "w+");
    if (file == NULL)
	{
        system_config_data_debug("打开文件失败");
        return 1;
    }
	system_config_data_debug("打开文件成功");
	fwrite(str, 1, len, file);
	system_config_data_debug("文件内容:\r\n%s\r\n",str);
    fclose(file);

	system_config_data_debug("保存配置文件结束");
	cJSON_Delete(root);
	return 0;
}
