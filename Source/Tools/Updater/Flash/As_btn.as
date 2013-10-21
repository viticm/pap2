package{
	
	import flash.display.MovieClip;
	import flash.display.Sprite;
	import flash.events.MouseEvent;
	import flash.events.Event;
	
	import flash.display.SimpleButton;	
	import flash.system.fscommand;
	import flash.net.navigateToURL;
    import flash.net.URLRequest;
	
	import flash.utils.Timer;
	import flash.events.TimerEvent;
	
	public class As_btn extends Sprite
	
	{
		
		public var rootA:Object;//主舞台根路径
		
		public var adMain:String = "ad1";
		
		private var cant_up_down:Boolean = true;
		
		private var speed_ad1_move:Number = 3;
		
		private var cantMove:Boolean = false;
		
		
		public function As_btn(){
			
		}//构造函数
		
		//气泡显示
		
		
		function btn_smallWindowOver(){
			rootA._pipo.x = 832;
			rootA._pipo.txt_pipo.text = "最小化"
			rootA._pipo.gotoAndPlay("over");
		}
		function btn_smallWindowOver_cpu1(){
			rootA._pipo.x = 832;
			rootA._pipo.txt_pipo.text = "最小化"
			rootA._pipo.gotoAndStop("overCpu1");
		}
		
		//web地址激活 开始
		
		function btnWebClick1(){
				navigateToURL(new URLRequest("http://my.xoyo.com/index.php?action=reg"),"_blank");
			}
		function btnWebClick2(){
				navigateToURL(new URLRequest("http://pay.kingsoft.com/"),"_blank");
			}
		function btnWebClick3(){
				navigateToURL(new URLRequest("http://jx3.xoyo.com/zt/xiazai/index.shtml"),"_blank");
			}
		function btnWebClick4(){
				navigateToURL(new URLRequest("http://jx3.bbs.xoyo.com/"),"_blank");
			}
		function btnWebClick5(){
				navigateToURL(new URLRequest("http://jx3.xoyo.com/"),"_blank");
			}
		function btnWebClick6(){
				navigateToURL(new URLRequest("http://kefu.xoyo.com/"),"_blank");
			}
			
		//web地址激活 结束
			
		//3大按钮 2 按钮 开始
		
		function btn_smallWindowClick(){ 
		
				rootA.call_MinWindow();
		}
		
		function btn_quitClick(){ 
			
			if (rootA.btn_use_quit == true){
				
				rootA.call_Quitupdate();
			}
			
		}
		
		function btn_configClick(){ 
		
			if (rootA.btn_use_config == true){
			
				rootA.call_Configgame();
				
			}
			
		}
			
		function btn_startGameClick(){ 
		
			if (rootA.btn_use_startGame == true){
			
				rootA.call_Startgame();
				
			}
			
		}
		
		//3大按钮 2按钮 结束
		
		//CLICK 共用
		function btn_ad1UpClick(){
			
			if(rootA.cpu_power == 1){
				
				ad1n_down_over_cpu1();
				
			}
				speed_ad1_move = 3;
					
			}
		function btn_ad1DownClick(){
				
				if(rootA.cpu_power == 1){
				
				ad1n_up_over_cpu1();
				
			}
					speed_ad1_move = 3;
				
			}
		
		function btnAd1Click(){
			
			if (rootA.cpu_power == 2){
				
				adMain = "ad1";
			}
			
			rootA._ad1.y = 216;
			
			rootA._btnMain.btn_ad1Down.alpha = 1;
			
			rootA._btnMain.btn_ad1Up.alpha = 0.1;
					
		}
		function btnAd2Click(){
			
			if (rootA.cpu_power == 2 ){
				
					adMain = "ad2";
					
			}
			
			if (rootA.logcantOpen == true){
				
				rootA.As_logActive();//调用打开log
			}
			if (rootA.logcantOpen == false){
				
				rootA.As_logErro();//调用打开log
			}
					
		}
		
		private function btnActive_doing(){
			
			const _btnMain:MovieClip  = rootA._btnMain;
			const btnWeb1:MovieClip = _btnMain.btnWeb1;
			const btnWeb2:MovieClip = _btnMain.btnWeb2;
			const btnWeb3:MovieClip = _btnMain.btnWeb3;
			const btnWeb4:MovieClip = _btnMain.btnWeb4;
			const btnWeb5:MovieClip = _btnMain.btnWeb5;
			const btnWeb6:MovieClip = _btnMain.btnWeb6;
			
			const btn_quit:MovieClip = _btnMain.btn_quit;
			const btn_config:MovieClip = _btnMain.btn_config;
			const btn_startGame:MovieClip = _btnMain.btn_startGame;
			
			const back_ad1_ad2:MovieClip = _btnMain.back_ad1_ad2;
			const lightBack:MovieClip = _btnMain.back_ad1_ad2.lightBack;
			
			const btn_quit_btn:Sprite = Sprite(_btnMain.btn_quit_btn);
			const btn_config_btn:Sprite = Sprite(_btnMain.btn_config_btn);
			const btn_startGame_btn:Sprite = Sprite(_btnMain.btn_startGame_btn);
			const btn_smallWindow:SimpleButton = _btnMain.btn_smallWindow;
			const btn_closeWindow:MovieClip = _btnMain.btn_closeWindow;
			
			const btnAd1:SimpleButton = _btnMain.btnAd1;
			const btnAd2:SimpleButton = _btnMain.btnAd2;
			
			const btn_ad1Up:MovieClip = _btnMain.btn_ad1Up;
			const btn_ad1Down:MovieClip = _btnMain.btn_ad1Down;
			
			const btn_ad1Up_btn:SimpleButton = _btnMain.btn_ad1Up_btn;
			const btn_ad1Down_btn:SimpleButton = _btnMain.btn_ad1Down_btn;
			
			const back_closeWindow:MovieClip = _btnMain.back_closeWindow;
			const pipo:MovieClip = rootA._pipo;
			
			function btn_closeWindowOver(){
				
				if (rootA.btn_use_quit == true){
					
					_btnMain.buttonMode = true;
				
					back_closeWindow.gotoAndPlay("over");
					
					pipo.x = 873;
					pipo.txt_pipo.text = "关闭"
					pipo.gotoAndPlay("over");
				
				}else{
					
					back_closeWindow.gotoAndStop(1);
					
					_btnMain.buttonMode = false;
				}
		}
		
		function btn_closeWindowOver_cpu1(){
			
			if (rootA.btn_use_quit == true){
					
					_btnMain.buttonMode = true;
				
					back_closeWindow.gotoAndStop("overCpu1");
					
					pipo.x = 873;
					pipo.txt_pipo.text = "关闭"
					pipo.gotoAndStop("overCpu1");
			
				}else{
					_btnMain.buttonMode = false;
				}
		}
		
		
		function btn_closeWindowOut(){//关闭
			
			if (rootA.btn_use_quit == true){
					
					_btnMain.buttonMode = true;
				
					back_closeWindow.gotoAndPlay("out");
					
					pipoOut();
				
			}else{
					_btnMain.buttonMode = false;
			}
			
			pipoOut();
			
		}
		
		function pipoOut(){
			
			pipo.x = 948;
			pipo.txt_pipo.text = " "
			pipo.gotoAndStop(1);
		}
		
		function btn_closeWindowOut_cpu1(){
			
			if (rootA.btn_use_quit == true){
					
					_btnMain.buttonMode = true;
				
					back_closeWindow.gotoAndStop("upcom");
					
					pipoOut();
				
				}else{
					_btnMain.buttonMode = false;
				}
			
			
			
		}
			
			
			//over c1 开始---------------------------
			function btn_quitOver_cpu1(){
			
				if (rootA.btn_use_quit == true){
					
					_btnMain.buttonMode = true;
				
					btn_quit.gotoAndStop("overCpu1");
				
				}else{
					_btnMain.buttonMode = false;
				}
			}
			
			function btn_configOver_cpu1(){
			
				if (rootA.btn_use_config == true){
					
					_btnMain.buttonMode = true;
					
					btn_config.gotoAndStop("overCpu1");
					
				}else{
					_btnMain.buttonMode = false;
				}
			
			}
			
			
			function btn_startGameOver_cpu1(){
			
				if (rootA.btn_use_startGame == true){
				
					_btnMain.buttonMode = true;
				
					btn_startGame.gotoAndStop("overCpu1");
				}else{
					_btnMain.buttonMode = false;
				}
			}
			
			
			function btnAd1Over_cpu1(){
				
					back_ad1_ad2.gotoAndStop("overCpu1_ad1");
				
			}
			function btnAd2Over_cpu1(){
				
					back_ad1_ad2.gotoAndStop("overCpu1_ad2");
				
			}
			function btn_ad1UpOver_cpu1(){
				
				btn_ad1Up.gotoAndStop("overCpu1");
				
					cantMove = true;
					
			}
			function btn_ad1DownOver_cpu1(){
				
				btn_ad1Down.gotoAndStop("overCpu1");
				
					cantMove = true;
					
			}
			
			
			function btnOver1_cpu1(){btnWeb1.gotoAndStop("overCpu1");}
				function btnOver2_cpu1(){btnWeb2.gotoAndStop("overCpu1");}
				function btnOver3_cpu1(){btnWeb3.gotoAndStop("overCpu1");}
				function btnOver4_cpu1(){btnWeb4.gotoAndStop("overCpu1");}
				function btnOver5_cpu1(){btnWeb5.gotoAndStop("overCpu1");}
				function btnOver6_cpu1(){btnWeb6.gotoAndStop("overCpu1");}
				
			//over c1 结束---------------------------
			
			//over c2 开始---------------------------
			
			function btn_quitOver(){
			
				if (rootA.btn_use_quit == true){
				
					_btnMain.buttonMode = true;
					
					btn_quit.gotoAndPlay("over");
				
					btn_quit.shanjiao.gotoAndPlay("over");
					
					rootA.stop_all();
					
				}else{
				
					_btnMain.buttonMode = false;
				
				}
			}
		
		
			function btn_configOver(){
			
				if (rootA.btn_use_config == true){
					
					_btnMain.buttonMode = true;
				
					btn_config.gotoAndPlay("over");
				
					btn_config.shanjiao.gotoAndPlay("over");
					
					rootA.stop_all();
					
				}else{
				
					_btnMain.buttonMode = false;
				
				}
			}
		
		
			function btn_startGameOver(){
			
				if (rootA.btn_use_startGame == true){
					
					_btnMain.buttonMode = true;
				
					btn_startGame.gotoAndPlay("over");
					
					btn_startGame.shanjiao.gotoAndPlay(2);
					
					rootA.stop_all();
					
				}else{
				
					_btnMain.buttonMode = false;
				
				}
			}
			
			function btnOver1(){btnWeb1.gotoAndPlay("over");}
			function btnOver2(){btnWeb2.gotoAndPlay("over");}
			function btnOver3(){btnWeb3.gotoAndPlay("over");}
			function btnOver4(){btnWeb4.gotoAndPlay("over");}
			function btnOver5(){btnWeb5.gotoAndPlay("over");}
			function btnOver6(){btnWeb6.gotoAndPlay("over");}
			
			function btnAd1Over(){
				
				if (adMain == "ad2"){
					
					back_ad1_ad2.gotoAndPlay("toad1");
					
				}
				
					rootA.stop_all();
			}
			function btnAd2Over(){
				
				if (adMain == "ad1"){
					
					back_ad1_ad2.gotoAndPlay("toad2");
					
				}
				
					rootA.stop_all();
			}
			function btn_ad1UpOver(){
				btn_ad1Up.gotoAndPlay("over");
				
					cantMove = true;
					
					ad1n_down_over();
					
					rootA.stop_all();
					
			}
			function btn_ad1DownOver(){
				btn_ad1Down.gotoAndPlay("over");
				
					cantMove = true;
					
					ad1n_up_over();
					
					rootA.stop_all();
					
			}
			
			//over c2 结束---------------------------
			
			_btnMain.buttonMode = true; //手形光标
			if (rootA.cpu_power == 1){
				
				_btnMain.addEventListener (MouseEvent.MOUSE_OVER, btnOver_cpu1); 
				function btnOver_cpu1(evt:MouseEvent){//----------------------------鼠标over
				
				switch(evt.target.name){
                	case "btnWeb1":
                    btnOver1_cpu1();
                	break;
                	case "btnWeb2":
                    btnOver2_cpu1();
                	break;
					case "btnWeb3":
                    btnOver3_cpu1();
                	break;
					case "btnWeb4":
                    btnOver4_cpu1();
                	break;
					case "btnWeb5":
                    btnOver5_cpu1();
                	break;
					case "btnWeb6":
                    btnOver6_cpu1();
                	break;
					
					case "btnAd1":
                  	btnAd1Over_cpu1();
                	break;
					case "btnAd2":
                  	btnAd2Over_cpu1();
                	break;
					
					case "btn_quit_btn":
					btn_quitOver_cpu1();
					break;
					
					case "btn_config_btn":
					btn_configOver_cpu1();
					break;
					
					case "btn_startGame_btn":
					btn_startGameOver_cpu1();
					break;
					
					case "btn_ad1Up_btn":
					btn_ad1UpOver_cpu1();
					break;
					
					case "btn_ad1Down_btn":
					btn_ad1DownOver_cpu1();
					break;
					
					case "btn_closeWindow":
					btn_closeWindowOver_cpu1();
					break;
					
					case "btn_smallWindow":
					btn_smallWindowOver_cpu1();
                	break;
					
            	}
       		}//btnOver() end
			
			}//if over c1 end
			
			if (rootA.cpu_power == 2){
				_btnMain.addEventListener (MouseEvent.MOUSE_OVER, btnOver); 
				function btnOver(evt:MouseEvent){//----------------------------鼠标over
				
				switch(evt.target.name){
                	case "btnWeb1":
                    btnOver1();
                	break;
                	case "btnWeb2":
                    btnOver2();
                	break;
					case "btnWeb3":
                    btnOver3();
                	break;
					case "btnWeb4":
                    btnOver4();
                	break;
					case "btnWeb5":
                    btnOver5();
                	break;
					case "btnWeb6":
                    btnOver6();
                	break;
					
					case "btnAd1":
                  	btnAd1Over();
                	break;
					case "btnAd2":
                  	btnAd2Over();
                	break;
					
					case "btn_quit_btn":
					btn_quitOver();
					break;
					
					case "btn_config_btn":
					btn_configOver();
					break;
					
					case "btn_startGame_btn":
					btn_startGameOver();
					break;
					
					case "btn_ad1Up_btn":
					btn_ad1UpOver();
					break;
					
					case "btn_ad1Down_btn":
					btn_ad1DownOver();
					break;
					
					case "btn_closeWindow":
					btn_closeWindowOver();
					break;
					
					case "btn_smallWindow":
					btn_smallWindowOver();
                	break;
					
            	}
       		}//btnOver() end
				
			}//if over c2 end
			
			//out c1 开始
			function btnOut1_cpu1(){btnWeb1.gotoAndStop(1);}
			function btnOut2_cpu1(){btnWeb2.gotoAndStop(1);}
			function btnOut3_cpu1(){btnWeb3.gotoAndStop(1);}
			function btnOut4_cpu1(){btnWeb4.gotoAndStop(1);}
			function btnOut5_cpu1(){btnWeb5.gotoAndStop(1);}
			function btnOut6_cpu1(){btnWeb6.gotoAndStop(1);}
			
			function btnAd1Out_cpu1(){
				
					back_ad1_ad2.gotoAndStop(1);
			}
			function btnAd2Out_cpu1(){
				
					back_ad1_ad2.gotoAndStop(1);
					
					if (rootA.logcantOpen == false){
				
						rootA._log_erro.gotoAndStop(1);
					}
			}
			function btn_quitOut_cpu1(){
				if (rootA.btn_use_quit == true){
				
					btn_quit.gotoAndStop("outCpu1");
				}
				_btnMain.buttonMode = true;
			}
			function btn_configOut_cpu1(){
			
				if (rootA.btn_use_config == true){
				
					btn_config.gotoAndStop("outCpu1");
				}
				_btnMain.buttonMode = true;
			}
			
			function btn_startGameOut_cpu1(){
			
				if (rootA.btn_use_startGame == true){
				
					btn_startGame.gotoAndStop("outCpu1");
				}
				_btnMain.buttonMode = true;
			}
			
			function btn_ad1UpOut_cpu1(){
				
				btn_ad1Up.gotoAndStop(1);
				
				speed_ad1_move = 1;
					
					cantMove = false;
					
				
			}
			function btn_ad1DownOut_cpu1(){
				btn_ad1Down.gotoAndStop(1);
				
					speed_ad1_move = 1;
					
					cantMove = false;
			}
			//out c1 结束
			
			//out c2 开始
			function btn_quitOut(){
				if (rootA.btn_use_quit == true){
				
					btn_quit.gotoAndPlay("out");
				
					btn_quit.shanjiao.gotoAndPlay("out");
					
					rootA.play_all();
				}
				_btnMain.buttonMode = true;
			}
			function btn_configOut(){
			
				if (rootA.btn_use_config == true){
				
					btn_config.gotoAndPlay("out");
				
					btn_config.shanjiao.gotoAndPlay("out");
					
					rootA.play_all();
				}
				_btnMain.buttonMode = true;
			}
			
			function btn_startGameOut(){
			
				if (rootA.btn_use_startGame == true){
				
					btn_startGame.gotoAndPlay("out");
					
					btn_startGame.shanjiao.gotoAndPlay("out");
					
					rootA.play_all();
				}
				_btnMain.buttonMode = true;
			}
			function btnOut1(){btnWeb1.gotoAndPlay("out");}
			function btnOut2(){btnWeb2.gotoAndPlay("out");}
			function btnOut3(){btnWeb3.gotoAndPlay("out");}
			function btnOut4(){btnWeb4.gotoAndPlay("out");}
			function btnOut5(){btnWeb5.gotoAndPlay("out");}
			function btnOut6(){btnWeb6.gotoAndPlay("out");}
			
			function btnAd1Out(){
				
				if (adMain == "ad2"){
					
					back_ad1_ad2.gotoAndPlay("toad2");
					
				}
				
					rootA.play_all();
			}
			function btnAd2Out(){
				
				if (adMain == "ad1"){
					
					back_ad1_ad2.gotoAndPlay("toad1");
					
					rootA.play_all();
				}
				
			}
			
			function btn_ad1UpOut(){
				
				btn_ad1Up.gotoAndPlay("out");
				
				speed_ad1_move = 1;
					
				cantMove = false;
				
				rootA.play_all();
					
			}
			function btn_ad1DownOut(){
				btn_ad1Down.gotoAndPlay("out");
				
					speed_ad1_move = 1;
					
					cantMove = false;
					
					rootA.play_all();
					
			}
			
			//out c2 结束
			if (rootA.cpu_power == 1){
				
				_btnMain.addEventListener (MouseEvent.MOUSE_OUT, btnOut_cpu1);
				function btnOut_cpu1(evt:MouseEvent){//----------------------------鼠标out
				
				switch(evt.target.name){
                	case "btnWeb1":
                    btnOut1_cpu1();
                	break;
                	case "btnWeb2":
                    btnOut2_cpu1();
                	break;
					case "btnWeb3":
                    btnOut3_cpu1();
                	break;
					case "btnWeb4":
                    btnOut4_cpu1();
                	break;
					case "btnWeb5":
                    btnOut5_cpu1();
                	break;
					case "btnWeb6":
                    btnOut6_cpu1();
                	break;
					case "btnAd1":
                  	btnAd1Out_cpu1();
                	break;
					case "btnAd2":
                  	btnAd2Out_cpu1();
                	break;
					
					case "btn_quit_btn":
					btn_quitOut_cpu1();
					break;
					
					case "btn_config_btn":
					btn_configOut_cpu1();
					break;
					
					case "btn_startGame_btn":
					btn_startGameOut_cpu1();
                	break;
					
					case "btn_ad1Up_btn":
					btn_ad1UpOut_cpu1();
					break;
					
					case "btn_ad1Down_btn":
					btn_ad1DownOut_cpu1();
					break;
					
					case "btn_closeWindow":
					btn_closeWindowOut_cpu1();
					break;
					
					case "btn_smallWindow":
					pipoOut();
                	break;
            	}
       		}//btnOut_cpu1() end
				
			}//if out c1 end
			
			if (rootA.cpu_power == 2){
				
				_btnMain.addEventListener (MouseEvent.MOUSE_OUT, btnOut);
				function btnOut(evt:MouseEvent){//----------------------------鼠标out
				
				switch(evt.target.name){
                	case "btnWeb1":
                    btnOut1();
                	break;
                	case "btnWeb2":
                    btnOut2();
                	break;
					case "btnWeb3":
                    btnOut3();
                	break;
					case "btnWeb4":
                    btnOut4();
                	break;
					case "btnWeb5":
                    btnOut5();
                	break;
					case "btnWeb6":
                    btnOut6();
                	break;
					case "btnAd1":
                  	btnAd1Out();
                	break;
					case "btnAd2":
                  	btnAd2Out();
                	break;
					
					case "btn_quit_btn":
					btn_quitOut();
					break;
					
					case "btn_config_btn":
					btn_configOut();
					break;
					
					case "btn_startGame_btn":
					btn_startGameOut();
                	break;
					
					case "btn_ad1Up_btn":
					btn_ad1UpOut();
					break;
					
					case "btn_ad1Down_btn":
					btn_ad1DownOut();
					break;
					
					case "btn_closeWindow":
					btn_closeWindowOut();
					break;
					
					case "btn_smallWindow":
					pipoOut();
                	break;
            	}
       		}//btnOver() end
				
			}//if out c2 end
			
			
			
			//----------------------mouse_down 共用
			
			_btnMain.addEventListener (MouseEvent.MOUSE_DOWN, btnMouseDown); 
			function btnMouseDown(evt:MouseEvent){//----------------------------鼠标Click
           		 switch(evt.target.name){
                	case "btnWeb1":
                    btnWebClick1();
                	break;
                	case "btnWeb2":
                    btnWebClick2();
                	break;
					case "btnWeb3":
                    btnWebClick3();
                	break;
					case "btnWeb4":
                    btnWebClick4();
                	break;
					case "btnWeb5":
                    btnWebClick5();
                	break;
					case "btnWeb6":
                    btnWebClick6();
                	break;
					case "btn_ad1Up_btn":
					btn_ad1UpClick();
					break;
					
					case "btn_ad1Down_btn":
					btn_ad1DownClick();
					break;
					
            	}
       		}
			
			//----------------------click 共用
			
			_btnMain.addEventListener (MouseEvent.CLICK, btnClick); 
			function btnClick(evt:MouseEvent){//----------------------------鼠标Click
           		 switch(evt.target.name){
                	
					case "btnAd1":
                  	btnAd1Click();
                	break;
					
					case "btnAd2":
                  	btnAd2Click();
                	break;
					
					case "btn_quit_btn":
					btn_quitClick();
					break;
					
					case "btn_config_btn":
					btn_configClick();
					break;
					
					case "btn_startGame_btn":
					btn_startGameClick();
					break;
					
					case "btn_closeWindow":
					btn_quitClick();
					break;
					
					case "btn_smallWindow":
					btn_smallWindowClick();
                	break;
            	}
       		}
			
		}//btnActive_doing_cpu1() --------------------------------------------end
		
	
		//---------------------------------------------- ** _ad1上下滚动控制
		
		private function ad1n_down_over_cpu1(){
			
			const ad1:Sprite = Sprite(rootA._ad1);
			
			const btn_ad1Up:Sprite = Sprite(rootA._btnMain.btn_ad1Up);
			
			const btn_ad1Down:Sprite = Sprite(rootA._btnMain.btn_ad1Down);
			
			ad1.y = 220;
			
			btn_ad1Down.alpha = 1;
			
			btn_ad1Up.alpha = 0.5;
		}//ad1n_down_over_cpu1() end
		
		private function ad1n_up_over_cpu1(){
			
			const ad1:Sprite = Sprite(rootA._ad1);
			
			const btn_ad1Up:Sprite = Sprite(rootA._btnMain.btn_ad1Up);
			
			const btn_ad1Down:Sprite = Sprite(rootA._btnMain.btn_ad1Down);
			
			ad1.y = 176;
			
			btn_ad1Up.alpha = 1;
			
			btn_ad1Down.alpha = 0.5;
		
		}//ad1n_down_over_cpu1() end
		
		private function ad1n_down_over(){//ad1往下走 回复促使位置
			
			const ad1n:Sprite = Sprite(rootA._ad1);
			
			const btn_ad1Up:Sprite = Sprite(rootA._btnMain.btn_ad1Up);
			
			const btn_ad1Down:Sprite = Sprite(rootA._btnMain.btn_ad1Down);
			
			const stop_y_complete:Number = 216;
			
			ad1n.addEventListener(Event.ENTER_FRAME,onEnterFrame_ad1nUp);
			function onEnterFrame_ad1nUp(event:Event){
				if (ad1n.y >= stop_y_complete){
					
					if (btn_ad1Up.alpha <= 0.3){
						
						ad1n.removeEventListener(Event.ENTER_FRAME, onEnterFrame_ad1nUp);
						
						btn_ad1Up.alpha = 0.3;
						
					}else{
						
						btn_ad1Up.alpha = btn_ad1Up.alpha - 0.1;
						
						
					}
					
					ad1n.y = stop_y_complete;
				}
				
				if (cantMove == false){
					
					ad1n.removeEventListener(Event.ENTER_FRAME, onEnterFrame_ad1nUp);
					
				}
				if (cantMove == true){
					
					ad1n.y = ad1n.y + speed_ad1_move * 2;
					
					if (btn_ad1Down.alpha >= 0.9){
						btn_ad1Down.alpha = 1;
					}else{
						
						btn_ad1Down.alpha = btn_ad1Down.alpha + 0.1;
					}
					
					
				}
				
			}//onEnterFrame_ad1nUp() end
		
		
		}//ad1n_down_over() end
		
		
		private function ad1n_up_over(){//ad1往上走
			
			const ad1n:Sprite = Sprite(rootA._ad1);
			
			const btn_ad1Up:Sprite = Sprite(rootA._btnMain.btn_ad1Up);
			
			const btn_ad1Down:Sprite = Sprite(rootA._btnMain.btn_ad1Down);
			
			const stop_y_complete:Number = 186;
			
			ad1n.addEventListener(Event.ENTER_FRAME,onEnterFrame_Down);
			function onEnterFrame_Down(event:Event){
				
				if (ad1n.y <= stop_y_complete){
					
					if (btn_ad1Down.alpha <= 0.3){
						
						ad1n.removeEventListener(Event.ENTER_FRAME, onEnterFrame_Down);
						
						btn_ad1Down.alpha = 0.3;
						
					}else{
						
						btn_ad1Down.alpha = btn_ad1Down.alpha - 0.1;
						
					}
					
					ad1n.y = stop_y_complete - 2;
					
				}
				
				if (cantMove == false){
					
					ad1n.removeEventListener(Event.ENTER_FRAME, onEnterFrame_Down);
					
				}
				
				if (cantMove == true){
					
					ad1n.y = ad1n.y - speed_ad1_move;
					
					if (btn_ad1Up.alpha >= 0.9){
						btn_ad1Up.alpha = 1;
					}else{
						btn_ad1Up.alpha = btn_ad1Up.alpha + 0.1;
					}
					
				}
				
			}//onEnterFrame_ad1nUp() end
		
		
		}//ad1n_down_over() end
		
		
		
		
		function btnActive(){
			
			btnActive_doing();
			
		}
		
		
	}//class AsColorChange end
	
	
}