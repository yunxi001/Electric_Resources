#include "wav.h"
#include "main.h"
#include "stdio.h"
#include "ff.h"

#define WAVEFILEBUFSIZE		512

//打开音乐文件，获得WAVE文件的参数
uint8_t wave_decode_init(char* fname,wavctrl* wavx)
{
	uint32_t br=0;
	uint8_t res=0;
	ChunkRIFF *riff;
	ChunkFMT *fmt;
	ChunkFACT *fact;
	ChunkDATA *data;

    FILE* file;
    char TempBuf[WAVEFILEBUFSIZE];

	file=fopen(fname, "r");											//打开文件
	if(file!=NULL)
	{
		WAV_DEBUG("打开文件成功:%s\r\n",fname);

        int count = fread(&TempBuf, WAVEFILEBUFSIZE, 1, file);

		//fread(&WavFile,TempBuf,WAVEFILEBUFSIZE/2,file);									//读取WAVEFILEBUFSIZE/2字节数据
		riff=(ChunkRIFF *)TempBuf;																			//获取RIFF块
		if(riff->Format==0X45564157)																		//是WAV文件
		{
				fmt=(ChunkFMT *)(TempBuf+12);																	//获取FMT块
				if(fmt->AudioFormat==1||fmt->AudioFormat==3)								//线性PCM或32位WAVE=3
				{
					fact=(ChunkFACT *)(TempBuf+12+8+fmt->ChunkSize);					//读取FACT块
					if(fact->ChunkID==0X74636166||fact->ChunkID==0X5453494C)
						wavx->datastart=12+8+fmt->ChunkSize+8+fact->ChunkSize;	//具有fact/LIST块的时候(未测试)
					else
						wavx->datastart=12+8+fmt->ChunkSize;
					data=(ChunkDATA *)(TempBuf+wavx->datastart);
					WAV_DEBUG("ChunkID:%x\r\n",data->ChunkID);
					WAV_DEBUG("ChunkSize:%d\r\n",data->ChunkSize);
					if(data->ChunkID==0X61746164)															//读取DATA块成功
					{
						wavx->audioformat=fmt->AudioFormat;											//音频格式
						wavx->nchannels=fmt->NumOfChannels;											//通道数
						wavx->samplerate=fmt->SampleRate;												//采样率
						wavx->bitrate=fmt->ByteRate*8;													//位速率=通道数×每秒数据位数×每样本的数据位数
						wavx->blockalign=fmt->BlockAlign;												//块对齐=通道数×每样本的数据位值／8
						wavx->bps=fmt->BitsPerSample;														//位数,8/16/24/32位
						wavx->datasize=data->ChunkSize;													//音频数据块大小
						wavx->datastart=wavx->datastart+8;											//数据流开始的地方.
						WAV_DEBUG("wavx->audioformat:%d\r\n",wavx->audioformat);
						WAV_DEBUG("wavx->nchannels(通道数):%d\r\n",wavx->nchannels);
						WAV_DEBUG("wavx->samplerate(采样率):%d\r\n",wavx->samplerate);
						WAV_DEBUG("wavx->bitrate:%d\r\n",wavx->bitrate);
						WAV_DEBUG("wavx->blockalign:%d\r\n",wavx->blockalign);
						WAV_DEBUG("wavx->bps(位数):%d\r\n",wavx->bps);
						WAV_DEBUG("wavx->datasize:%d\r\n",wavx->datasize);
						WAV_DEBUG("wavx->datastart:%d\r\n",wavx->datastart);
					}
					else
						res=4;																									//data区域未找到.
				}
				else
					res=3;																										//非线性PCM，不支持
		}
		else
        {

            res=2;
        }
																															//非wav文件
	}
	else
    {
        WAV_DEBUG("打开文件失败:%s\r\n",fname);
        res=1;
    }
	fclose(file);																														//打开文件错误
	return res;
}


int volume_adjust(short  * in_buf, short  * out_buf, float in_vol)
{
    int i, tmp;

    // in_vol[0, 100]
    float vol = in_vol - 98;

    if(-98<vol && vol<0)
        vol = 1/(vol*(-1));
    else if(0<=vol && vol<=1)
        vol = 1;
    /*
    else if(1<=vol && vol<=2)
        vol = vol;
    */
    else if(vol<=-98)
        vol = 0;
    else if(vol>=2)
        vol = 40;  //这个值可以根据你的实际情况去调整

    tmp = (*in_buf)*vol; // 上面所有关于vol的判断，其实都是为了此处*in_buf乘以一个倍数，你可以根据自己的需要去修改

    // 下面的code主要是为了溢出判断
    if(tmp > 32767)
        tmp = 32767;
    else if(tmp < -32768)
        tmp = -32768;
    *out_buf = tmp;

    return 0;
}


//得到当前播放时间
//fx:文件指针
//wavx:wav播放控制器
void wave_get_curtime(FILE*fx,wavctrl *wavx)
{
	long long fpos;
 	wavx->totsec=wavx->datasize/(wavx->bitrate/8);	//歌曲总长度(单位:秒)
	//fpos=fx->fptr-wavx->datastart; 					//得到当前文件播放到的地方
	 fpos = ftell(fx);//得到当前文件播放到的地方
	wavx->cursec=fpos*wavx->totsec/wavx->datasize;	//当前播放到第多少秒了?

}
//wav文件快进快退函数
//pos:需要定位到的文件位置
//返回值:当前文件位置(即定位后的结果)
// u32 wav_file_seek(u32 pos)
// {
// 	u8 temp;
// 	if(pos>audiodev.file->fsize)
// 	{
// 		pos=audiodev.file->fsize;
// 	}
// 	if(pos<wavctrl.datastart)pos=wavctrl.datastart;
// 	if(wavctrl.bps==16)temp=8;	//必须是8的倍数
// 	if(wavctrl.bps==24)temp=12;	//必须是12的倍数
// 	if((pos-wavctrl.datastart)%temp)
// 	{
// 		pos+=temp-(pos-wavctrl.datastart)%temp;
// 	}
// 	f_lseek(audiodev.file,pos);
// 	return audiodev.file->fptr;
// }

uint32_t wav_buffill(FILE* file,uint8_t *buf,uint16_t size,uint8_t bits)
{
	uint16_t readlen=0;
	uint32_t ReadSize;
	uint16_t i;
	uint8_t *p;
    uint8_t tbuf[1024];
	if(bits==24)//24bit音频,需要处理一下
	{
		readlen=(size/4)*3;							//此次要读取的字节数
        ReadSize = fread(tbuf, 1,size, file);
		//fread(file,tbuf,readlen,(UINT*)&ReadSize);	//读取数据
		p=tbuf;
		for(i=0;i<size;)
		{
			buf[i++]=p[1];
			buf[i]=p[2];
			i+=2;
			buf[i++]=p[0];
			p+=3;
		}
		ReadSize=(ReadSize*4)/3;		//填充后的大小.
	}else
	{
		//f_read(file,buf,size,(UINT*)&ReadSize);//16bit音频,直接读取数据
        ReadSize = fread(buf, 1,size, file);
		if(ReadSize<size)//不够数据了,补充0
		{
			for(i=ReadSize;i<size-ReadSize;i++)buf[i]=0;
		}
		// int indata=0,outdata=0;
		// for(i=0;i<size;i+=2)
		// {
		// 	indata = buf[i]<<8|buf[i+1];
		// 	//volume_adjust(&indata,&outdata,100);
		// 	outdata = indata*0.5;
		// 	buf[i]=outdata<<8;
		// 	buf[i+1] = outdata;
		// }




	}
	return ReadSize;
}



uint32_t FillWaveFileBuf(FILE* file,uint8_t *Buf,uint16_t Size,uint8_t nchannels,uint8_t bits)
{
	uint32_t NeedReadSize=0;
	uint32_t ReadSize;
	uint32_t i;
	uint8_t *p;
	float *f;
	int sound;
	uint8_t TempBuf[1024];
	//先放左声道，再放右声道，I2S传输时，先传数据的高位，再传低位以16位为单位传输
	//双声道
	if(nchannels==2)
	{
		if(bits==16)																	//16bit数据直接读取
		{
			//f_read(&WavFile,Buf,Size,(UINT*)&ReadSize);
			ReadSize = fread(Buf, 1,Size, file);
//			WAV_DEBUG("读取长度:%d\r\n",ReadSize);
		}
		else if(bits==24)														  //24bit音频,需要调整读取数据与DMA缓存中的顺序
		{
			NeedReadSize=(Size/4)*3;																//此次要读取的字节数
			//f_read(&WavFile,TempBuf,NeedReadSize,(UINT*)&ReadSize);	//读取数据
			ReadSize = fread(TempBuf, 1,Size, file);
			p=TempBuf;
			ReadSize=(ReadSize/3)*4;																//填充后的大小.
			for(i=0;i<ReadSize;)
			{
				Buf[i]=p[1];
				Buf[i+1]=p[2];
				Buf[i+2]=0;
				Buf[i+3]=p[0];
				i+=4;
				p+=3;
			}
		}
		else if(bits==8)																		//8bit音频数据要转换为16位模式进行播放
		{
			NeedReadSize=Size/2;																		//此次要读取的字节数
			//f_read(&WavFile,TempBuf,NeedReadSize,(UINT*)&ReadSize);	//读取数据
			ReadSize = fread(TempBuf, 1,Size, file);
			p=TempBuf;
			ReadSize=ReadSize*2;																		//填充后的大小.
			for(i=0;i<ReadSize;)
			{
				Buf[i]=0;
				Buf[i+1]=*p+0x80;
				p++;
				i=i+2;
			}
		}
		else																											//32bit WAVE 浮点数-1至1表示声音
		{
			//f_read(&WavFile,TempBuf,Size,(UINT*)&ReadSize);					//读取数据
			ReadSize = fread(TempBuf, 1,Size, file);
			f=(float*)TempBuf;
			for(i=0;i<ReadSize;)
			{
				sound=0x7FFFFFFF*(*f);
				Buf[i]=(uint8_t)(sound>>16);
				Buf[i+1]=(uint8_t)(sound>>24);
				Buf[i+2]=(uint8_t)(sound);
				Buf[i+3]=(uint8_t)(sound>>8);
				f++;
				i=i+4;
			}
		}
	}
	//单声道，调整为双声道数据进行播放
	else
	{
		if(bits==16)
		{
			NeedReadSize=Size/2;																		//此次要读取的字节数
			//f_read(&WavFile,TempBuf,NeedReadSize,(UINT*)&ReadSize);	//读取数据
			ReadSize = fread(TempBuf, 1,Size, file);
			p=TempBuf;
			ReadSize=ReadSize*2;																		//填充后的大小.
			for(i=0;i<ReadSize;)
			{
				Buf[i]=p[0];
				Buf[i+1]=p[1];
				Buf[i+2]=p[0];
				Buf[i+3]=p[1];
				i+=4;
				p=p+2;
			}
		}
		else if(bits==24)																	//24bit音频
		{
			NeedReadSize=(Size/8)*3;																//此次要读取的字节数
			//f_read(&WavFile,TempBuf,NeedReadSize,(UINT*)&ReadSize);	//读取数据
			ReadSize = fread(TempBuf, 1,Size, file);
			p=TempBuf;
			ReadSize=(ReadSize/3)*8;																//填充后的大小.
			for(i=0;i<ReadSize;)
			{
				Buf[i]=p[1];
				Buf[i+1]=p[2];
				Buf[i+2]=0;
				Buf[i+3]=p[0];
				Buf[i+4]=p[1];
				Buf[i+5]=p[2];
				Buf[i+6]=0;
				Buf[i+7]=p[0];
				p+=3;
				i+=8;
			}

		}
		else if(bits==8)																		//8bit音频
		{
			NeedReadSize=Size/4;																		//此次要读取的字节数
			//f_read(&WavFile,TempBuf,NeedReadSize,(UINT*)&ReadSize);	//读取数据
			ReadSize = fread(TempBuf, 1,Size, file);
			p=TempBuf;
			ReadSize=ReadSize*4;																		//填充后的大小.
			for(i=0;i<ReadSize;)
			{
				Buf[i]=0;
				Buf[i+1]=*p+0x80;
				Buf[i+2]=0;
				Buf[i+3]=*p+0x80;
				i+=4;
				p++;
			}
		}
		else																											//32bit
		{
			NeedReadSize=Size/2;																		//此次要读取的字节数
			//f_read(&WavFile,TempBuf,NeedReadSize,(UINT*)&ReadSize);	//读取数据
			ReadSize = fread(TempBuf, 1,Size, file);
			f=(float*)TempBuf;
			ReadSize=ReadSize*2;																		//填充后的大小.
			for(i=0;i<ReadSize;)
			{
				sound=0x7FFFFFFF*(*f);
				Buf[i+4]=Buf[i]=(uint8_t)(sound>>16);
				Buf[i+5]=Buf[i+1]=(uint8_t)(sound>>24);
				Buf[i+6]=Buf[i+2]=(uint8_t)(sound);
				Buf[i+7]=Buf[i+3]=(uint8_t)(sound>>8);
				f++;
				i=i+8;
			}
		}
	}
	if(ReadSize<Size)																						//不够数据了,补充0
	{

		for(i=ReadSize;i<Size-ReadSize;i++)
			Buf[i]=0;
	}
	//WAV_DEBUG("ReadSize=%d\r\n",ReadSize);
	return ReadSize;
}






