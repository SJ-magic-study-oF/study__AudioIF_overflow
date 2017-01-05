#study__AudioIF_overflow

##環境
*	OS X El Capitan(10.11.6)
*	Xcode : 7.2
*	oF : 0.9.0

##add on


##Contents
AutoMagic Realtimeの事前検討.  
audioIn()/ audioOut()での処理について研究し、  
overflowなくrealtime処理ができるように準備する。  
  
結論としては、  
重い処理を全て、audioOut()側に持って来ればOK.  
audioIn()側に持ってくると、overflowを起こす前に、monitor soundが消えてしまう。  
詳しい原因は、不明のまま。。。   


##Device
*	Audio I/F  
	EDITOL by Roland.  
*	Amp  
	BEHRINGER ヘッドフォンアンプ HA400


##note






