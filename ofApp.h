/************************************************************
[最重要:考察]
*	audioIn()は最低限のdata格納とし、audioOut()側に重めの処理を全て持ってくる。
	"stream over/underflow detected"は、audioIn()とaudioOut()の合計がsamplingTimeをoverした時に発生。
	But...audioIn()に重い処理を入れると、"stream over/underflow detected"が発生していないのに、Monitor音(audioOutにて格納)が消える問題が発生。
	おそらく、無音でOKならば、処理は問題なくこなせるが、Monitor音も欲しいので、audioOut()側に重い処理を全て持ってくることにする。
	
	audioOut()側では、samplingTime内に処理が完了すれば、次のaudioIn()->audioOut()へと問題なく移行することをusleep()関数にて確認済み。
	

*	Sound Cardから、音が欲しい時にaudioOut()が呼ばれるが、audioIn()が処理中だと、audioOut()が待たされる。
	一定時間以内にoutputに格納しないと、音が出ない様子。
	前の周に格納したdataを出力しても良さそうだが、これもしてくれない。
	timeoutがあって、これ以内に格納しないと音を出さないのかな？
	
	ただ、この「一定時間」は、起動ごと、或いは状態が変化するなど、
	ちょっとしたtimingによって変わる。
	この点については、調査したものの、不明のまま。


	
[その他:考察]
*	以下のようにしておくのが安全ぽい

	void ofApp::exit()
	{
		soundStream.stop();
		soundStream.close();
	}


*	audioIn() -> audioOut()の順

	つまり、
	ofAppとofSoundStreamは、別threadで動くが、
	audioIn()とaudioOut()は、同時に動かないので、変数の共有などが可。
	
	How I checked
		t_in_AudioIn, t_in_AudioOut をそれぞれの関数の頭で記録し、
		処理量をほぼzeroとした時に、
		1. audioIn()でprintf(); した時
			t_in_AudioOut + samplingTime = t_in_AudioIn.
			samplingTime:23ms when 44.1kHz, BufferSize = 1024
			
		2. audioOut()でprintf(); した時
			t_in_AudioIn ≒ t_in_AudioOut.
		
	
		
*	"stream over/underflow detected"を出力しているのは、audioOut().
	soundStream.setup()
	にてinのみ/outのみ にした上でwait timeを伸ばして行って確認。
	
	
************************************************************/


/************************************************************
[same description as "study__AudioIF"]

■fix in ofSoundStream
URL
	https://forum.openframeworks.cc/t/ofsoundstream-different-sound-device-for-input-and-output/11087/9

purpose
	different audio devices for audio input and output.
	
fix files
	/Users/nobuhiro/Documents/source/openframeworks/of_v0.9.0_osx_release/libs/openFrameworks/sound/
	上記URLでは、"ofBaseSoundStream.cpp"となっているが、正しくは、"ofSoundStream.cpp".
		ofBaseSoundStream.h
		ofSoundStream.cpp
		ofSoundStream.h
	
fix point
	ofBaseSoundStream.h
		add 2 lines.
			virtual void setInDeviceID(int deviceID) = 0;  
			virtual void setOutDeviceID(int deviceID) = 0;  
	
	ofSoundStream.cpp
		add 2 functions.
			void ofSoundStream::setInDeviceID(int deviceID){  
				if( soundStream ){  
					soundStream->setInDeviceID(deviceID);  
				}  
			}  
			
			void ofSoundStream::setOutDeviceID(int deviceID){  
				if( soundStream ){  
					soundStream->setOutDeviceID(deviceID);  
				}  
			}  
	
	ofSoundStream.h
		add 2 lines.
			void setInDeviceID(int deviceID);  
			void setOutDeviceID(int deviceID);  
			
	
How to use
	soundStream.setInDeviceID(InDevID);
	soundStream.setOutDeviceID(OutDevID);

■Audio I/F
input:cable
	基本モノラル(多分)。
	LRはもちろん、Hot/Cold/GNDなどの表記があるか、注意して見ること。
	
	つまり、walkmanからinputする時は、LR(白赤)に分けて入れること。
	ちなみに、「標準ステレオ」をinputしたつもりで動作させると、
	「低音スカスカなんだけど」って感じになった。
	
input:Level
	walkmanから入れる場合、cable上のLevelはLine Level。
	なので、input感度は、minimumに設定すること。
	
	

■primitive study
	■DTM初心者のためのオーディオインターフェイス選び 2016
		http://www.dtmstation.com/archives/51971048.html
	
	■「ｄＢ（デシベル）その１～音圧レベル」～音響豆知識(２)
		http://blogs.yahoo.co.jp/boorankenko/43674244.html
	
	■「ｄＢ（デシベル）その２～ラインレベルのいろいろ」～音響豆知識（３）
		http://blogs.yahoo.co.jp/boorankenko/43695409.html	
			mic Level/ Line Level/ Speaker(or Amp) Level
		
	■「ｄＢ（デシベル）その３～マイクレベル」～音響豆知識（４）
		http://blogs.yahoo.co.jp/boorankenko/43753018.html
	
	
	■オーディオ信号レベル
		http://detail.chiebukuro.yahoo.co.jp/qa/question_detail/q1413957214
			マイク 1.6mV
			エレキギター 200mV
			ライン 1,230mV（1,000mV）
			ヘッドフォン、スピーカー 2,400-2.830mV

■【今さら聞けない用語シリーズ】
	■初心者向け【今さら聞けない用語シリーズ】初めてのオーディオ・インターフェース選び
		http://info.shimamura.co.jp/digital/knowledge/2014/06/21591
		
	■【今さら聞けない用語シリーズ】3分でわかる！インピーダンスって何でしょう？ ロー出しハイ受け？
		http://info.shimamura.co.jp/digital/knowledge/2014/01/16805
		
	■【今さら聞けない用語シリーズ】3分でわかる！オーディオ・ケーブルの種類とバランス・アンバランスの違い
		http://info.shimamura.co.jp/digital/knowledge/2014/01/16501
		
	■【今さら聞けない用語シリーズ】音と周波数
		http://info.shimamura.co.jp/digital/knowledge/2014/03/19260
		
	■【今さら聞けない用語シリーズ】3分でわかる！コンデンサーマイクとダイナミックマイクの違い おすすめマイク
		http://info.shimamura.co.jp/digital/knowledge/2014/04/23017
************************************************************/
#pragma once

/************************************************************
************************************************************/
#include "ofMain.h"

/************************************************************
************************************************************/
class ofApp : public ofBaseApp{
private:
	/****************************************
	****************************************/
	enum{
		// AUDIO_BUF_SIZE = 512,
		AUDIO_BUF_SIZE = 1024,
		
		AUDIO_BUFFERS = 2,
		
		AUDIO_SAMPLERATE = 44100,
	};
	enum{
		WIDTH	= 400,
		HEIGHT	= 400,
	};

	/****************************************
	****************************************/
	int soundStream_Input_DeviceId;
	int soundStream_Output_DeviceId;
	ofSoundStream soundStream;
	
	vector<float> samplesLeft;
	vector<float> samplesRight;
	
	/********************
	********************/
	bool b_sleep_in;
	bool b_sleep_out;
	
	int t_sleep_us;
	const int t_sleepStep_us;
	const int t_sleep_Min_us;
	
	float t_in_AudioIn;
	float t_in_AudioOut;
	float t_Duration_AudioIn;
	float t_Duration_AudioOut;
	
	/****************************************
	****************************************/
	void audioIn(float *input, int bufferSize, int nChannels);
	void audioOut(float *output, int bufferSize, int nChannels);
	
	
public:
	/****************************************
	****************************************/
	ofApp(int _soundStream_Input_DeviceId, int _soundStream_Output_DeviceId);
	~ofApp();
	void exit();
	
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
