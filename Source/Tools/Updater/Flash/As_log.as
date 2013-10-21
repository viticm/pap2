package{
	
	import flash.display.MovieClip;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.events.KeyboardEvent;
	import flash.display.SimpleButton;
	
	public class As_log extends Sprite 
	
	{
		public var rootA:Object;//主舞台根路径
		
		public var speed_logOpen:uint = 3; //卷轴打开速度
		
		public var croll_log:Number = 407.35; //卷轴筒滚动启动位置
		
		public function As_log(){
			
		}//构造函数
		public function log_erro_active(){
			
			import flash.utils.Timer;
			
			if (rootA.cpu_power == 2){
				
				rootA._versionBg.gotoAndPlay("over");//文字装饰闪光一划而过
			}
			
			rootA.txt_version.text = "日志文件载入出错";
			
			var erroTimer:Timer = new Timer(3000,1);
			
			function erroShow(){
					
					erroTimer.stop();
					
					erroTimer.removeEventListener("timer",erroShow);
					
					if (rootA.cpu_power == 2){
				
						rootA._versionBg.gotoAndPlay("over");
					
					}
					rootA.txt_version.text = rootA.txt_version_content;
					
					rootA.btn_ad1_2_ad1();
					
			}
			
			erroTimer.addEventListener("timer",erroShow);
			
			erroTimer.start();
		}
		
		private function log_backOpen(){
			
			if (rootA.log_opened == true){
			
			rootA.log_opened = false;
			
			const log:Sprite = Sprite(rootA._log);
			const juanUp:MovieClip = rootA._log.juanUp;
			const juanDown:MovieClip = rootA._log.juanDown;
			const juanMiddle:Sprite = Sprite(rootA._log.juanMiddle);
			
			log.y = 290;
				
			juanUp.x = juanDown.x = -2;
			juanMiddle.x = -1.6;
			juanMiddle.y = -3.4;
				
			juanUp.y = -13;
			juanDown.y = 13;
			juanMiddle.height = 1;
				
			log.alpha = 1;
				
			var distance_juanUp_y:Number;
			var distance_juanDown_y:Number;
			var distance_juanMiddle_height:Number;
				
			const target_juanUp_y:Number = -261.5;
			const target_juanDown_y:Number = 261.5;
			const target_juanMiddle_height:Number = 507.4;

			function onEnterFrame_open(event:Event):void {
					
				if(juanMiddle.height == croll_log){
					juanUp.gotoAndPlay("open");
					juanDown.gotoAndPlay("open");
				}
				if (juanMiddle.height >= target_juanMiddle_height - 0.2){
					log.removeEventListener(Event.ENTER_FRAME, onEnterFrame_open);
						
					juanUp.y = target_juanUp_y ;
					juanDown.y = target_juanDown_y;
					juanMiddle.height = target_juanMiddle_height;
					
					rootA.log_opened = true;
					
					rootA.drag_scrollOn();
				
					rootA.As_scrollOpen();//--------------** 打开滚动日志
					
					speed_logOpen = 3;
				
					croll_log = 407.35;
					
						
				}else{
					distance_juanUp_y = target_juanUp_y - juanUp.y;
					juanUp.y += distance_juanUp_y / speed_logOpen;
					
					distance_juanDown_y = target_juanDown_y - juanDown.y;
					juanDown.y += distance_juanDown_y / speed_logOpen;
						
					distance_juanMiddle_height = target_juanMiddle_height - juanMiddle.height;
					juanMiddle.height += distance_juanMiddle_height / speed_logOpen;
					
				}
			}//onEnterFrame_open() end
			
			log.addEventListener(Event.ENTER_FRAME, onEnterFrame_open);
			
		
			}//if end
			
		}//log_backOpen() end
		
		function logActive(){
			
			if (rootA.cpu_power == 2){
				
				rootA.stop_all();
			}
			
			log_backOpen();
			
		}
		
		private function log_backClose_doing_cpu1(){
			
			rootA.log_opened = false;
			const log:Sprite = Sprite(rootA._log);
			const juanUp:MovieClip = rootA._log.juanUp;
			const juanDown:MovieClip = rootA._log.juanDown;
			const juanMiddle:Sprite = Sprite(rootA._log.juanMiddle);
				
				log.alpha = 1;
				
				const speed:Number = 4;
				const speedy:Number = 4;
				var distance_juanUp_y:Number;
				var distance_juanDown_y:Number;
				var distance_juanMiddle_height:Number;
				
				var distance_log_y:Number;
				
				const target_juanUp_y:Number = -13;
				const target_juanDown_y:Number = 13;
				const target_juanMiddle_height:Number = 1;
				
				const target_log_y:Number = 430;
				
				juanUp.gotoAndPlay("close");
				juanDown.gotoAndPlay("close");

				function onEnterFrame_close(event:Event):void {
					
					if (juanMiddle.height <= 1.25){
						
						log.removeEventListener(Event.ENTER_FRAME, onEnterFrame_close);
						juanUp.stop();
						juanDown.stop();
						
						log.y = 618;
						rootA.log_opened = true;
						
						rootA.drag_scrollOff();
						
					}else{
					
						distance_juanUp_y = target_juanUp_y - juanUp.y;
						juanUp.y += distance_juanUp_y / speed;
						
						distance_juanDown_y = target_juanDown_y - juanDown.y;
						juanDown.y += distance_juanDown_y / speed;
						
						distance_juanMiddle_height = target_juanMiddle_height - juanMiddle.height;
						juanMiddle.height += distance_juanMiddle_height / speed;
						
						distance_log_y = target_log_y - log.y;
						log.y += distance_log_y / speedy;
						
					}
						
			    }//onEnterFrame_close() end
				
				log.addEventListener(Event.ENTER_FRAME, onEnterFrame_close);
				
		}//log_backClose_doing_cpu1() end
		
		private function log_backClose_doing(){
			
			rootA.log_opened = false;
			const log:Sprite = Sprite(rootA._log);
			const juanUp:MovieClip = rootA._log.juanUp;
			const juanDown:MovieClip = rootA._log.juanDown;
			const juanMiddle:Sprite = Sprite(rootA._log.juanMiddle);
				
				log.alpha = 1;
				
				const speed:Number = 4;
				const speedy:Number = 4;
				var distance_juanUp_y:Number;
				var distance_juanDown_y:Number;
				var distance_juanMiddle_height:Number;
				var distance_alpha:Number;
				
				var distance_log_y:Number;
				
				const target_juanUp_y:Number = -13;
				const target_juanDown_y:Number = 13;
				const target_juanMiddle_height:Number = 1;
				const target_alpha:Number = 0;
				
				const target_log_y:Number = 430;
				
				juanUp.gotoAndPlay("close");
				juanDown.gotoAndPlay("close");

				function onEnterFrame_close(event:Event):void {
					
					if (juanMiddle.height <= 1.25){
						
						log.removeEventListener(Event.ENTER_FRAME, onEnterFrame_close);
						juanUp.stop();
						juanDown.stop();
						
						log.y = 618;
						rootA.log_opened = true;
						
						rootA.drag_scrollOff();
						
					}else{
					
						distance_juanUp_y = target_juanUp_y - juanUp.y;
						juanUp.y += distance_juanUp_y / speed;
						
						distance_juanDown_y = target_juanDown_y - juanDown.y;
						juanDown.y += distance_juanDown_y / speed;
						
						distance_juanMiddle_height = target_juanMiddle_height - juanMiddle.height;
						juanMiddle.height += distance_juanMiddle_height / speed;
						
						distance_log_y = target_log_y - log.y;
						log.y += distance_log_y / speedy;
						
						distance_alpha = target_alpha - log.alpha;
						log.alpha += distance_alpha / 5;
						
					}
						
			    }//onEnterFrame_close() end
				
				log.addEventListener(Event.ENTER_FRAME, onEnterFrame_close);
				
		}//log_backClose() end
		
		function log_backClose(){
			
			if (rootA.cpu_power == 1){
				
				log_backClose_doing_cpu1();
				
			}else if(rootA.cpu_power == 2){
				
				log_backClose_doing();
				
			}
			
				
		}
				
		private function log_btnActive_cool_doing(){
			const btnQuit_x:SimpleButton = rootA._log.btnQuit_x;
				
				const btnQuit_font:MovieClip = rootA._log.btnQuit_font;
				
				btnQuit_x.x = 147.4;
				btnQuit_x.y = -238.4;
				btnQuit_font.x = 110;
				btnQuit_font.y = 235;
				
				function btnQuitOver(){
					
					btnQuit_font.gotoAndPlay("over");
				}
				function btnQuitOut(){
					
					btnQuit_font.gotoAndPlay("out");
				}
				btnQuit_font.addEventListener(MouseEvent.MOUSE_OVER, btnQuitOver); 
				btnQuit_font.addEventListener(MouseEvent.MOUSE_OUT, btnQuitOut); 
				
				
				function btnQuitClick(){
					if (rootA.log_opened == true){
						
						btnQuit_font.removeEventListener(MouseEvent.MOUSE_OVER, btnQuitOver); 
						btnQuit_font.removeEventListener(MouseEvent.MOUSE_OUT, btnQuitOut); 
						btnQuit_x.removeEventListener(MouseEvent.CLICK, btnQuitClick); 
						btnQuit_font.removeEventListener(MouseEvent.CLICK, btnQuitClick);
						
						rootA.As_scrollClose();
						
						
						rootA.stage.removeEventListener(KeyboardEvent.KEY_DOWN, keyDownHandler);
					}
				}
				
				function keyDownHandler(event:KeyboardEvent):void {
   
   					switch(event.keyCode){
    	
   						case 27:
    
   						btnQuitClick();
				
   				 		break;
    
   			 		}
   
       		 }//keyDownHandler() end

				btnQuit_x.addEventListener(MouseEvent.CLICK, btnQuitClick); 
				btnQuit_font.addEventListener(MouseEvent.CLICK, btnQuitClick); 
				rootA.stage.addEventListener(KeyboardEvent.KEY_DOWN, keyDownHandler);//添加一个键盘事件
			
		}
		
		function log_btnActive_cool(){//冷却log的两个关闭按钮
			
				log_btnActive_cool_doing();
				
		}//log_btnActive_Cool() end
		
	}//class AsColorChange end
	
	
}