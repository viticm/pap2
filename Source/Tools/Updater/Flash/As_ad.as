package{
	
	import flash.display.MovieClip;
	import flash.display.Sprite;
	import flash.display.Loader;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.events.ProgressEvent;
	import flash.events.IOErrorEvent;
	import flash.events.TimerEvent;
	import flash.net.navigateToURL;
    import flash.net.URLRequest;
	import flash.net.URLLoader;
	import flash.utils.Timer;
	import flash.text.TextField;
	
	public class As_ad extends Sprite 
	
	{
		public var rootA:Object;//主舞台根路径
		
		private var xml_data1:XML  = <xml></xml>;

		private var xml_data2:XML = <xml></xml>;//包含图片信息
		
		private var xmlLoaded1:Boolean = false;
		
		private var xmlLoaded2:Boolean = false;
		
		private var ad1TimeReload:uint = 1;
		
		private var lightTride_needX:Number = 0;;
		private var lightTride_needY:Number = 0;
		private var lightTride_oldX:Number = 0;;
		private var lightTride_oldY:Number = 0;
		
		public function As_ad(){ }//构造函数
		
			
		//log更新日志载入给全局变量rootA.xml_data_log
		private function xml_log_load(){
			
			var xml_loader:URLLoader = new URLLoader(new URLRequest(rootA.url_log));
			
			function loading(event:ProgressEvent) {
				
				//rootA._ad1.erro_web.txt.text = "下载更新日志，已完成  "+ Math.round ((event.bytesLoaded/event.bytesLoaded)*100)+"%\n";
					
			}
			function loadError(event:IOErrorEvent) {
				
				rootA.logcantOpen = false;
				
				ad1_xml_load1();
				
				Gc_this();
			}
			function loaded(event:Event) {
				
				
				var xml_data:XML = XML(xml_loader.data);
				
				rootA.xml_data_log = xml_data;
				
				rootA.As_scrollActive();//激活弹出日志，暂时不显示;
				
				Gc_this();
			}
			
			function Gc_this(){
				
				//释放资源
				xml_loader.removeEventListener(Event.COMPLETE, loaded);
				xml_loader.removeEventListener(ProgressEvent.PROGRESS, loading);
				xml_loader.removeEventListener(IOErrorEvent.IO_ERROR, loadError);
				
				xml_loader = null;
				
				rootA.url_log = null;
			}
			
			xml_loader.addEventListener(Event.COMPLETE, loaded);
			xml_loader.addEventListener(ProgressEvent.PROGRESS, loading);
			xml_loader.addEventListener(IOErrorEvent.IO_ERROR, loadError);
				
		}//xml_log() end
		
		
		private function webTxtActive(){
					
					var lightTride:MovieClip = rootA._ad1.lightTride;
					
					const txtY:uint = 90;
					const txtH:uint = 22;
					
					var xml1_y:Array = new Array(txtY, txtY + txtH, txtY + txtH*2, txtY + txtH*3, txtY + txtH*4);
					
					var xml2_y:Array = new Array(txtY, txtY + txtH, txtY + txtH*2, txtY + txtH*3, txtY + txtH*4);
					
					var xml1_x:uint = 53;
					
					var xml2_x:uint = 245;
					
					var mySpriteList : Array = new Array();
					
					var webTxt_x:Array = new Array(xml1_x, xml2_x , xml1_x, xml2_x, xml1_x, xml2_x, xml1_x, xml2_x, xml1_x, xml2_x);
					var webTxt_y:Array = new Array(xml1_y[0], xml2_y[0], xml1_y[1], xml2_y[1], xml1_y[2], xml2_y[2], xml1_y[3], xml2_y[3], xml1_y[4], xml2_y[4]);
					
					var webTxt_txt:Array = new Array(xml_data1.child("sever")[0], xml_data2.child("news")[0], xml_data1.child("sever")[1], xml_data2.child("news")[1], xml_data1.child("sever")[2], xml_data2.child("news")[2], xml_data1.child("sever")[3], xml_data2.child("news")[3], xml_data1.child("sever")[4], xml_data2.child("news")[4]);
					
					var webTxt_color:Array = new Array(xml_data1.child("sever")[0].@c, xml_data2.child("news")[0].@c, xml_data1.child("sever")[1].@c, xml_data2.child("news")[1].@c, xml_data1.child("sever")[2].@c, xml_data2.child("news")[2].@c, xml_data1.child("sever")[3].@c, xml_data2.child("news")[3].@c, xml_data1.child("sever")[4].@c, xml_data2.child("news")[4].@c);
					
					var webTxt_tx:Array = new Array(xml_data1.child("sever")[0].@tx, xml_data2.child("news")[0].@tx, xml_data1.child("sever")[1].@tx, xml_data2.child("news")[1].@tx, xml_data1.child("sever")[2].@tx, xml_data2.child("news")[2].@tx, xml_data1.child("sever")[3].@tx, xml_data2.child("news")[3].@tx, xml_data1.child("sever")[4].@tx, xml_data2.child("news")[4].@tx);
					
					var webTxt_name:Array = new Array("webTxt0","webTxt1","webTxt2","webTxt3","webTxt4","webTxt5","webTxt6","webTxt7","webTxt8","webTxt9");
					var btnWeb_name:Array = new Array("btnWeb0","btnWeb1","btnWeb2","btnWeb3","btnWeb4","btnWeb5","btnWeb6","btnWeb7","btnWeb8","btnWeb9");
					for(var i : int = 0; i < 10; i++){
   						mySpriteList["webTxt"+i] = new webTxt_mc();
						
						addChild(mySpriteList["webTxt"+i]);
						
						mySpriteList["webTxt"+i].x = webTxt_x[i];
						mySpriteList["webTxt"+i].y = webTxt_y[i];
						
						mySpriteList["webTxt"+i].webTxt_mc_txt.txt.text = webTxt_txt[i];
						mySpriteList["webTxt"+i].webTxt_mc_txt.txt.textColor = webTxt_color[i];
						
						mySpriteList["btnWeb"+i] = new btnWeb_mc();
						mySpriteList["btnWeb"+i].x = mySpriteList["webTxt"+i].x;
						mySpriteList["btnWeb"+i].y = mySpriteList["webTxt"+i].y+1;
						
						addChild(mySpriteList["btnWeb"+i]);
						
						if (rootA.cpu_power == 2){
							mySpriteList["webTxt"+i].gotoAndPlay(String(webTxt_tx[i]));
						}
						
						mySpriteList["webTxt"+i].name = webTxt_name[i];
						mySpriteList["btnWeb"+i].name = btnWeb_name[i];
						
						//trace(webTxt_tx[i]);
						
					} 
					
			function btnWebOver0(){
				
				lightTride_needX = webTxt_x[0];
				lightTride_needY = webTxt_y[0];
				lightTrideMove();
				
			}
			function btnWebOver1(){
				
				lightTride_needX = webTxt_x[1];
				lightTride_needY = webTxt_y[1];
				lightTrideMove();
			}
			function btnWebOver2(){
				
				lightTride_needX = webTxt_x[2];
				lightTride_needY = webTxt_y[2];
				lightTrideMove();
			}
			function btnWebOver3(){
				
				lightTride_needX = webTxt_x[3];
				lightTride_needY = webTxt_y[3];
				lightTrideMove();
			}
			function btnWebOver4(){
				
				lightTride_needX = webTxt_x[4];
				lightTride_needY = webTxt_y[4];
				lightTrideMove();
			}
			function btnWebOver5(){
				
				lightTride_needX = webTxt_x[5];
				lightTride_needY = webTxt_y[5];
				lightTrideMove();
			}
			function btnWebOver6(){
				
				lightTride_needX = webTxt_x[6];
				lightTride_needY = webTxt_y[6];
				lightTrideMove();
			}
			function btnWebOver7(){
				
				lightTride_needX = webTxt_x[7];
				lightTride_needY = webTxt_y[7];
				lightTrideMove();
			}
			function btnWebOver8(){
				
				lightTride_needX = webTxt_x[8];
				lightTride_needY = webTxt_y[8];
				lightTrideMove();
			}
			function btnWebOver9(){
				
				lightTride_needX = webTxt_x[9];
				lightTride_needY = webTxt_y[9];
				lightTrideMove();
			}
			
			var ad1:Sprite = rootA._ad1;
			ad1.buttonMode = true; //手形光标
			
			//-------------over c1 kaishi
			
			function btnWebOver0_cpu1(){
				
				lightTride_needX = webTxt_x[0];
				lightTride_needY = webTxt_y[0];
				lightTrideMove_cpu1();
				
			}
			function btnWebOver1_cpu1(){
				
				lightTride_needX = webTxt_x[1];
				lightTride_needY = webTxt_y[1];
				lightTrideMove_cpu1();
			}
			function btnWebOver2_cpu1(){
				
				lightTride_needX = webTxt_x[2];
				lightTride_needY = webTxt_y[2];
				lightTrideMove_cpu1();
			}
			function btnWebOver3_cpu1(){
				
				lightTride_needX = webTxt_x[3];
				lightTride_needY = webTxt_y[3];
				lightTrideMove_cpu1();
			}
			function btnWebOver4_cpu1(){
				
				lightTride_needX = webTxt_x[4];
				lightTride_needY = webTxt_y[4];
				lightTrideMove_cpu1();
			}
			function btnWebOver5_cpu1(){
				
				lightTride_needX = webTxt_x[5];
				lightTride_needY = webTxt_y[5];
				lightTrideMove_cpu1();
			}
			function btnWebOver6_cpu1(){
				
				lightTride_needX = webTxt_x[6];
				lightTride_needY = webTxt_y[6];
				lightTrideMove_cpu1();
			}
			function btnWebOver7_cpu1(){
				
				lightTride_needX = webTxt_x[7];
				lightTride_needY = webTxt_y[7];
				lightTrideMove_cpu1();
			}
			function btnWebOver8_cpu1(){
				
				lightTride_needX = webTxt_x[8];
				lightTride_needY = webTxt_y[8];
				lightTrideMove_cpu1();
			}
			function btnWebOver9_cpu1(){
				
				lightTride_needX = webTxt_x[9];
				lightTride_needY = webTxt_y[9];
				lightTrideMove_cpu1();
			}
			
			//-------------over c1 jieshu
			
			//if over c1 开始
			if (rootA.cpu_power == 1){
				ad1.addEventListener (MouseEvent.MOUSE_OVER, btnWebOver_cpu1); 
				function btnWebOver_cpu1(evt:MouseEvent){//----------------------------鼠标over
				
				switch(evt.target.name){
                	case "btnWeb0":
                    btnWebOver0_cpu1();
                	break;
                	case "btnWeb1":
                    btnWebOver1_cpu1();
                	break;
					case "btnWeb2":
                    btnWebOver2_cpu1();
                	break;
					case "btnWeb3":
                    btnWebOver3_cpu1();
                	break;
					case "btnWeb4":
                    btnWebOver4_cpu1();
                	break;
					case "btnWeb5":
                    btnWebOver5_cpu1();
					break;
					case "btnWeb6":
                    btnWebOver6_cpu1();
					break;
					case "btnWeb7":
                    btnWebOver7_cpu1();
					break;
					case "btnWeb8":
                    btnWebOver8_cpu1();
					break;
					case "btnWeb9":
                    btnWebOver9_cpu1();
					break;
					
					
            	}
       		}//btnWebOver_cpu1() end
			}
			//if over c1 结束
			
			
			//if over c2 开始
			if (rootA.cpu_power == 2){
				
				ad1.addEventListener (MouseEvent.MOUSE_OVER, btnWebOver); 
				function btnWebOver(evt:MouseEvent){//----------------------------鼠标over
					
					rootA.stop_all();
					
				switch(evt.target.name){
                	case "btnWeb0":
                    btnWebOver0();
                	break;
                	case "btnWeb1":
                    btnWebOver1();
                	break;
					case "btnWeb2":
                    btnWebOver2();
                	break;
					case "btnWeb3":
                    btnWebOver3();
                	break;
					case "btnWeb4":
                    btnWebOver4();
                	break;
					case "btnWeb5":
                    btnWebOver5();
					break;
					case "btnWeb6":
                    btnWebOver6();
					break;
					case "btnWeb7":
                    btnWebOver7();
					break;
					case "btnWeb8":
                    btnWebOver8();
					break;
					case "btnWeb9":
                    btnWebOver9();
					break;
					
					
            	}
       		}//btnOver() end
			
				
			}//if over c2 开始
			
		
			function lightTrideMove_cpu1(){
				
				lightTride.gotoAndStop("overCpu1");
				
				lightTride.x = lightTride_needX;
				lightTride.y = lightTride_needY;
					
			}//lightTrideMove() end
			
			function lightTrideMove(){
				
				if (lightTride_oldY == lightTride_needY){
					lightTride.gotoAndPlay("over");
				}
				if (lightTride_oldY < lightTride_needY){
					lightTride.gotoAndPlay("down");
				}
				if (lightTride_oldY > lightTride_needY){
					lightTride.gotoAndPlay("up");
				}
					lightTride.x = lightTride_needX;
					lightTride.y = lightTride_needY;
					
			}//lightTrideMove() end
			function oldXY(){
				if (rootA.cpu_power == 2){
					
					lightTride.gotoAndPlay("out");
					
					lightTride_oldX = lightTride_needX;
					lightTride_oldY = lightTride_needY;
						
				}
				lightTride.gotoAndStop(1);
				
			}
			
			ad1.addEventListener (MouseEvent.MOUSE_OUT, btnWebOut); 
			function btnWebOut(evt:MouseEvent){//----------------------------鼠标over
				switch(evt.target.name){
                	case "btnWeb0":
                    btnWebOut0();
                	break;
                	case "btnWeb1":
                    btnWebOut0();
                	break;
					case "btnWeb2":
                    btnWebOut0();
                	break;
					case "btnWeb3":
                    btnWebOut0();
                	break;
					case "btnWeb4":
                    btnWebOut0();
                	break;
					case "btnWeb5":
                    btnWebOut0();
                	break;
					case "btnWeb6":
                    btnWebOut0();
                	break;
					case "btnWeb7":
                    btnWebOut0();
                	break;
					case "btnWeb8":
                    btnWebOut0();
                	break;
					case "btnWeb9":
                    btnWebOut0();
                	break;
                	
            	}
       		}//btnOver() end
			function btnWebOut0(){
				if (rootA.cpu_power == 2){
					
					rootA.play_all();
				}
				oldXY();
			}
			
			function btnWeb0(){navigateToURL(new URLRequest(xml_data1.child("sever")[0].@herf),"_blank");}
			function btnWeb1(){navigateToURL(new URLRequest(xml_data2.child("news")[0].@herf),"_blank");}
			function btnWeb2(){navigateToURL(new URLRequest(xml_data1.child("sever")[1].@herf),"_blank");}
			function btnWeb3(){navigateToURL(new URLRequest(xml_data2.child("news")[1].@herf),"_blank");}
			function btnWeb4(){navigateToURL(new URLRequest(xml_data1.child("sever")[2].@herf),"_blank");}
			function btnWeb5(){navigateToURL(new URLRequest(xml_data2.child("news")[2].@herf),"_blank");}
			
			function btnWeb6(){navigateToURL(new URLRequest(xml_data1.child("sever")[3].@herf),"_blank");}
			function btnWeb7(){navigateToURL(new URLRequest(xml_data2.child("news")[3].@herf),"_blank");}
			function btnWeb8(){navigateToURL(new URLRequest(xml_data1.child("sever")[4].@herf),"_blank");}
			function btnWeb9(){navigateToURL(new URLRequest(xml_data2.child("news")[4].@herf),"_blank");}
			
			function btnWebClick(evt:MouseEvent){//----------------------------鼠标over
				
				switch(evt.target.name){
                	case "btnWeb0":
                    btnWeb0();
                	break;
                	case "btnWeb1":
                    btnWeb1();
                	break;
					case "btnWeb2":
                    btnWeb2();
                	break;
					case "btnWeb3":
                    btnWeb3();
                	break;
					case "btnWeb4":
                    btnWeb4();
                	break;
					case "btnWeb5":
                    btnWeb5();
                	break;
					case "btnWeb6":
                    btnWeb6();
                	break;
					case "btnWeb7":
                    btnWeb7();
                	break;
					case "btnWeb8":
                    btnWeb8();
                	break;
					case "btnWeb9":
                    btnWeb9();
                	break;
					
            	}
       		}//btnOver() end
			ad1.addEventListener (MouseEvent.CLICK, btnWebClick); 
			
			
				rootA.As_webTxt_add();
				
			}//webTxtActive() end
		
		
		
		private function btn_slide_active(){
			
			const slide:MovieClip = rootA._ad1.slide;
			
			function btnClick1(){
				
				slide.gotoAndStop("hd1");
				
				ad1TimeReload = 0;
				
			}
			function btnClick2(){slide.gotoAndStop("hd2");ad1TimeReload = 0;}
			function btnClick3(){slide.gotoAndStop("hd3");ad1TimeReload = 0;}
			function btnClick_pic1(){
				
				navigateToURL(new URLRequest(xml_data2.child("pic")[0].@herf),"_blank");
			}
			function btnClick_pic2(){
				
				navigateToURL(new URLRequest(xml_data2.child("pic")[1].@herf),"_blank");
			}
			function btnClick_pic3(){
				
				navigateToURL(new URLRequest(xml_data2.child("pic")[2].@herf),"_blank");
			}
			
			function btnClick(Event:MouseEvent){
							switch(Event.target.name){
								case "btn1":
									btnClick1();
								break;
               					case "btn2":
                    				btnClick2();
                				break;
								case "btn3":
									btnClick3();
								break;
               					case "btn_pic1":
                    				btnClick_pic1();
                				break;
								case "btn_pic2":
									btnClick_pic2();
								break;
               					case "btn_pic3":
                    				btnClick_pic3();
                				break;
                				
           					}//switch() end
						
       			 		}//btnClick() end
						
			slide.addEventListener(MouseEvent.CLICK, btnClick); 
			
			hd_play();
			
		}//btn_slide_active() end
		
		private function hd_time_move_cpu1(){
			const slide:MovieClip = rootA._ad1.slide;
			
			const jiankangGonggao:MovieClip = rootA.jiangkang;
			
			var ad1Timer:Timer = new Timer(4300);
			
			var t:uint = 0;
			
			function ad1ChangePic(){
				
				
				if(ad1TimeReload == 0){
					
					ad1Timer.stop();
					
					ad1TimeReload = 1;
					
					ad1Timer.start();
					
			}else if(ad1TimeReload == 1){
					
					t ++;
					
					if (t == 4){
						
						t = 1
						
						slide.gotoAndStop("hd1");
					}else if(t == 3){
						
						slide.gotoAndStop("hd3");
					}else if(t == 2){
						
						slide.gotoAndStop("hd2");
					}else if(t == 1){
						
						slide.gotoAndStop("hd1");
					}
					
			}
				
				
			}//ad1ChangePic() end
			
			
			
			ad1Timer.addEventListener("timer",ad1ChangePic);
			
			ad1Timer.start();
			
		}//hd_time_move_cpu1() end
		
		private function hd_time_move(){
			
			const slide:MovieClip = rootA._ad1.slide;
			
			const jiankangGonggao:MovieClip = rootA.jiangkang;
			
			var ad1Timer:Timer = new Timer(4000);
			
			if (ad1TimeReload != 0 &&ad1Timer.currentCount == 0){
				
				ad1Timer.addEventListener("timer",ad1ChangePic);
			
				ad1Timer.start();
						
			}
			
			function ad1ChangePic(event:TimerEvent):void { 
				
				if(ad1Timer.currentCount == 1){
					
					ad1Timer.delay = 4500;
					
					ad1Timer.delay = 3500;
				}
			
				 if(ad1TimeReload == 0 && ad1Timer.running == true && ad1Timer.currentCount > 0){
					
					ad1Timer.stop();
					
					ad1TimeReload = 1;
					
					ad1Timer.start();
					
				}else{
					
					slide.play();
					
					jiankangGonggao.play();
					
				}
			
      			
    		}//ad1ChangePic() end
			
			
			
			
		}//hd_time_move() end
		
		private function ad1PicLoad(){
				
				var slide:Sprite = Sprite(rootA._ad1.slide);
				var hd1:Sprite = Sprite(rootA._ad1.slide.hd1);
				var hd2:Sprite = Sprite(rootA._ad1.slide.hd2);
				var hd3:Sprite = Sprite(rootA._ad1.slide.hd3);
				
				var ad1PicLoader1:Loader = new Loader();
				var ad1PicLoader2:Loader = new Loader();
				var ad1PicLoader3:Loader = new Loader();
				
				ad1PicLoader1.load(new URLRequest(xml_data2.child("pic")[0]));
				ad1PicLoader2.load(new URLRequest(xml_data2.child("pic")[1]));
				ad1PicLoader3.load(new URLRequest(xml_data2.child("pic")[2]));
				
				function loadError(event:IOErrorEvent){
					
				}
				
				function picLoading(event:ProgressEvent){
					
				}
				
				function picLoaded1(event:Event){
					
					hd1.addChild(ad1PicLoader1);
					ad1PicLoader1.contentLoaderInfo.removeEventListener(Event.COMPLETE, picLoaded1);
					ad1PicLoader1.contentLoaderInfo.removeEventListener(ProgressEvent.PROGRESS, picLoading);
					ad1PicLoader1.contentLoaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, loadError);
					
				}
				function picLoaded2(event:Event){
					
					hd2.addChild(ad1PicLoader2);
					
					ad1PicLoader2.contentLoaderInfo.removeEventListener(Event.COMPLETE, picLoaded2);
					ad1PicLoader2.contentLoaderInfo.removeEventListener(ProgressEvent.PROGRESS, picLoading);
					ad1PicLoader2.contentLoaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, loadError);
				}
				function picLoaded3(event:Event){
					
					hd3.addChild(ad1PicLoader3);
					
					btn_slide_active();
					
					ad1PicLoader3.contentLoaderInfo.removeEventListener(Event.COMPLETE, picLoaded3);
					ad1PicLoader3.contentLoaderInfo.removeEventListener(ProgressEvent.PROGRESS, picLoading);
					ad1PicLoader3.contentLoaderInfo.removeEventListener(IOErrorEvent.IO_ERROR, loadError);
					
				}
				
				ad1PicLoader1.contentLoaderInfo.addEventListener(Event.COMPLETE, picLoaded1);
				ad1PicLoader2.contentLoaderInfo.addEventListener(Event.COMPLETE, picLoaded2);
				ad1PicLoader3.contentLoaderInfo.addEventListener(Event.COMPLETE, picLoaded3);
				
				ad1PicLoader1.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, picLoading);
				ad1PicLoader2.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, picLoading);
				ad1PicLoader3.contentLoaderInfo.addEventListener(ProgressEvent.PROGRESS, picLoading);
				
				ad1PicLoader1.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, loadError);
				ad1PicLoader2.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, loadError);
				ad1PicLoader3.contentLoaderInfo.addEventListener(IOErrorEvent.IO_ERROR, loadError);
				
			}//ad1PicLoad() end
			
		private function ad1_xml_load2(){
			
			xmlLoaded2 = false;
			
			const xml_url2:URLRequest = new URLRequest(rootA.url_xml2);//导入客服数据文件
			
			const xml_loader2:URLLoader = new URLLoader(xml_url2);
			
			function loading2(event:ProgressEvent){
				rootA._ad1.erro_web.txt.text = "下载网页数据中...";
			}
			function loadError2(event:IOErrorEvent){
				
				rootA._ad1.erro_web.txt.text = "下载网页数据 2 失败！";
			}
			
			function loaded2(event:Event){
				
				xml_data2 = XML(xml_loader2.data);//news
				
				xmlLoaded2 = true;
				
				load_all_ready();
				
				xml_loader2.removeEventListener(Event.COMPLETE, loaded2);
				xml_loader2.removeEventListener(ProgressEvent.PROGRESS, loading2);
				xml_loader2.removeEventListener(IOErrorEvent.IO_ERROR, loadError2);
				
			}
			
			xml_loader2.addEventListener(Event.COMPLETE, loaded2);
			xml_loader2.addEventListener(ProgressEvent.PROGRESS, loading2);
			xml_loader2.addEventListener(IOErrorEvent.IO_ERROR, loadError2);
			
		}//ad1_xml_load2() end
		
		
		private function ad1_xml_load1(){
			
			xmlLoaded1 = false;
			
			const xml_url1:URLRequest = new URLRequest(rootA.url_xml1);//导入客服数据文件
			
			const xml_loader1:URLLoader = new URLLoader(xml_url1);
			
			function loading1(event:ProgressEvent){
				
				rootA._ad1.erro_web.txt.text = "下载网页数据中...";
			}
			function loadError1(event:IOErrorEvent){
				
				rootA._ad1.erro_web.txt.text = "下载网页数据 1 失败！";
				
			}
			
			function loaded1(event:Event){
				
				xml_data1 = XML(xml_loader1.data);//sever
				
				xmlLoaded1 = true;
				
				load_all_ready();
				
				xml_loader1.removeEventListener(Event.COMPLETE, loaded1);
				xml_loader1.removeEventListener(ProgressEvent.PROGRESS, loading1);
				xml_loader1.removeEventListener(IOErrorEvent.IO_ERROR, loadError1);
				
			}
			
			xml_loader1.addEventListener(Event.COMPLETE, loaded1);
			xml_loader1.addEventListener(ProgressEvent.PROGRESS, loading1);
			xml_loader1.addEventListener(IOErrorEvent.IO_ERROR, loadError1);
			
		}//ad1_xml_load1() end
		
		private function load_all_ready(){
			
			if (xmlLoaded2 == true && xmlLoaded1 == true){
				
				rootA._ad1.erro_web.txt.text = " ";
				
				rootA._ad1.erro_web.txt.x = 988;
				
				rootA.webLoading.x = -908;
				
				rootA.webLoading.gotoAndStop(1);
				
				ad1PicLoad();
						
				webTxtActive();
				
				rootA.url_xml1 = null;
				
				rootA.url_xml2 = null;
			}
			if (xmlLoaded1 == false){
				
				ad1_xml_load1();
				
				
				
			}
			if (xmlLoaded2 == false){
				
				ad1_xml_load2();
				
				
			}
			
			
		}
			
		function hd_play(){
			ad1TimeReload = 1;
			
			if (rootA.cpu_power == 1){
				
				hd_time_move_cpu1();
			
			}else if(rootA.cpu_power == 2){
				
				hd_time_move();
			}
			
			
		}
		function hd_stop(){
			
			ad1TimeReload = 2;
			
			hd_time_move();
		}
		
		function adActive(){
			
			ad1_xml_load1();//左边信息载入并处理
				
			ad1_xml_load2();//右边和顶部信息载入并处理
				
			xml_log_load();//更新日志载入
			
			rootA.webLoading.x = 342;
			rootA.webLoading.gotoAndPlay(2);
				
			
		}//adActive() end
		
		
	}//class AsColorChange end
	
	
}