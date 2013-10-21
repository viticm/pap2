package{
	
	import flash.display.MovieClip;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.utils.getTimer;
	import flash.display.SimpleButton;
	import flash.geom.Rectangle;
	
	public class As_scroll extends Sprite 
	
	{
		public var rootA:Object;//主舞台根路径
		
		private var logData:XML;
		
		public var speed_scrollOpen:uint = 4;
		
		public function As_scroll(){
			
			
		}//构造函数
		
	private function scrollOpen_doing_cpu1():void{
			
		const scrollBack:Sprite = Sprite(rootA._log.scrollBack1);
		
		const mask_scroll:Sprite = Sprite(rootA._log.scrollBack1.mask1);
		scrollBack.alpha = 1;
		scrollBack.y = -229.3;
		
		mask_scroll.y = -573.9;
				
		rootA.As_log_btnActive();
		
	}//scrollOpen_doing_cpu1() end
	
	private function scrollOpen_doing():void{
		
		rootA.As_log_btnActive();
		
		const scrollBack:Sprite = Sprite(rootA._log.scrollBack1);
		scrollBack.alpha = 1;
		scrollBack.y = -229.3;
		
		const mask_scroll:Sprite = Sprite(rootA._log.scrollBack1.mask1);
		mask_scroll.y = -473.9;
		
		scrollBack.mask = mask_scroll;
		
		const target_y:Number = 0;
		var distance_y:Number;
		
		mask_scroll.addEventListener(Event.ENTER_FRAME,onEnterFrame_maskMoving);
		function onEnterFrame_maskMoving(event:Event){
			if (mask_scroll.y == 0){
				
				mask_scroll.removeEventListener(Event.ENTER_FRAME, onEnterFrame_maskMoving);
			
				scrollBack.mask = null;
				
				mask_scroll.y = -573.9;
				
			}else{
				
				distance_y = target_y - mask_scroll.y;
				mask_scroll.y += distance_y / speed_scrollOpen;
			
			}
				
		}//onEnterFrame_maskMoving() end
		
	}//scrollOpen() end
	
	
	private function scrollClose_doing_cpu1():void{
		
		const scrollBack:MovieClip = rootA._log.scrollBack1;
		const mask_scroll:Sprite = Sprite(rootA._log.scrollBack1.mask1);
		
		const btnQuit_font:MovieClip = rootA._log.btnQuit_font;
		const btnQuit_x:SimpleButton = rootA._log.btnQuit_x;
		
		scrollBack.alpha = 1;
		mask_scroll.y = 0;
		mask_scroll.alpha = 0;
				
		scrollBack.y = 688;
		
		btnQuit_font.y = 688;
		btnQuit_x.y = 688;
				
		rootA.As_logClose();
		
	}//scrollClose() end
	
	private function scrollClose_doing():void{
		
		const scrollBack:MovieClip = rootA._log.scrollBack1;
		const mask_scroll:Sprite = Sprite(rootA._log.scrollBack1.mask1);
		
		const btnQuit_font:MovieClip = rootA._log.btnQuit_font;
		const btnQuit_x:SimpleButton = rootA._log.btnQuit_x;
		
		scrollBack.alpha = 1;
		mask_scroll.y = 0;
		mask_scroll.alpha = 1;
		
		scrollBack.mask = mask_scroll;
		
		btnQuit_font.y = 688;
		btnQuit_x.y = 688;
		
		const target_alpha:Number = 0;
		var distance_alpha:Number;
		
		const speed:Number = 2;
		mask_scroll.addEventListener(Event.ENTER_FRAME,onEnterFrame_alpha);
		function onEnterFrame_alpha(event:Event){
			if (mask_scroll.alpha <= 0.1){
				
				mask_scroll.removeEventListener(Event.ENTER_FRAME, onEnterFrame_alpha);
				mask_scroll.alpha = 0;
				scrollBack.mask = null;
				
				scrollBack.y = 688;
				
				rootA.As_logClose();
				
			}else{
				
				distance_alpha = target_alpha - mask_scroll.alpha;
				mask_scroll.alpha += distance_alpha / speed;
				scrollBack.alpha = mask_scroll.alpha;
				
			}
				
		}//onEnterFrame_maskMoving() end
		
		
	}//scrollClose() end
	
		//**滚动条---------------------------------------------------------------------------------------------------------------------开始
		
	private function scrollActive_doing(){
		
		const scrollBack:MovieClip = rootA._log.scrollBack1;
	
		scrollBack.x = -163.9;
		scrollBack.y = 680;
		
		var height_fc : uint = 250;
		var width_fc : uint = 0;
		
		//--------------------滚动条_sprite，滚动条按钮和滑块mc，被滚动的文本域初始化
		const scrollText:TextField = TextField(scrollBack.scrollText);
		const scrollBar_sprite:Sprite = Sprite(scrollBack.scrollBar);
		const up_btn:SimpleButton = SimpleButton(scrollBack.scrollBar.up_btn);
		const down_btn:SimpleButton = SimpleButton(scrollBack.scrollBar.down_btn);
		const pole_sprite:Sprite = Sprite(scrollBack.scrollBar.pole_mc);
		const bg_sprite:Sprite = Sprite(scrollBack.scrollBar.bg_mc);
		
		const back_paint:MovieClip = rootA._log.scrollBack1.back_paint;
		
		
		var poleStartHeight : Number;
		var poleStartY : Number;
		var totalPixels : Number;
		var isSelect : Boolean;
		////上下滚动按钮按钮下时间
		var putTime : Number;
		
		//——————可用性控制
		pole_sprite.visible = false;
		up_btn.enabled = false;
		down_btn.enabled = false;
		
		//——————其他属性初始化
		bg_sprite.useHandCursor = false;
		isSelect = scrollText.selectable;
		
		if(height_fc == 0) {
			
			bg_sprite.height = scrollText.height;
			
		}else {
			bg_sprite.height = height_fc;

		}
			
		if(width_fc != 0) { 
			bg_sprite.width = width_fc+2;
			pole_sprite.width=width_fc;
			up_btn.width = up_btn.height = down_btn.width = down_btn .height = width_fc;	
			
		}
		//down_btn.y = bg_sprite.y + bg_sprite.height - down_btn.height - 1;
		poleStartHeight = Math.floor(down_btn.y - up_btn.y - up_btn.height);
		poleStartY = pole_sprite.y = Math.floor(up_btn.y + up_btn.height);
			
		//——————注册侦听器
		//文本滚动与鼠标滚轮
		scrollText.addEventListener(Event.SCROLL, textScroll);
		scrollText.addEventListener(MouseEvent.MOUSE_WHEEL, mouseWheel);
			
		//上滚动按钮
		up_btn.addEventListener(MouseEvent.MOUSE_DOWN, upBtn);
		up_btn.stage.addEventListener(MouseEvent.MOUSE_UP, upBtnUp);
			
		//下滚动按钮
		down_btn.addEventListener(MouseEvent.MOUSE_DOWN, downBtn);
		down_btn.stage.addEventListener(MouseEvent.MOUSE_UP, downBtnUp);
			
		//滑块
		pole_sprite.addEventListener(MouseEvent.MOUSE_DOWN, poleSprite);
		pole_sprite.stage.addEventListener(MouseEvent.MOUSE_UP, poleUp);
			
		//滑块背景点击
		bg_sprite.addEventListener(MouseEvent.MOUSE_DOWN, bgDown);
			
		function textScroll(event : Event): void {//--------------文本滚动事件
		  
			//判断滑块儿是否显示，并根据文本内容多少定义滑块高度
			if(scrollText.maxScrollV != 1) {
				pole_sprite.visible = true;
				up_btn.enabled = true;
				down_btn.enabled = true;
				//定义一个高度元素，此元素随加载文本的增多，将无限趋向于1
				var heightVar : Number = 1 - (scrollText.maxScrollV - 1) / scrollText.maxScrollV;
				//根据高度因子初始化滑块的高度
				
				pole_sprite.height = Math.floor(poleStartHeight * Math.pow(heightVar, 1 / 3));
				totalPixels = Math.floor(down_btn.y - up_btn.y - up_btn.height - pole_sprite.height);
				pole_sprite.y = Math.floor(poleStartY + totalPixels * (scrollText.scrollV - 1) / (scrollText.maxScrollV - 1));
				
				back_paint.gotoAndStop("dis_half");//背景人物图片呈现
				
			}else {
				
				pole_sprite.visible = false;
				up_btn.enabled = false;
				down_btn.enabled = false;
				
			}
		}//textScroll() end
		
		
		scrollText.htmlText = rootA.xml_data_log;//---------------------------------------------------写入日志！！！
		
		function poleSprite(event : MouseEvent) : void {//--------------滑块滚动
		
			//首先取消文本框滚动侦听，因为文本滚动的时候会设置滑块的位置，而此时是通过滑块调整文本的位置，所以会产生冲突
			scrollText.removeEventListener(Event.SCROLL, textScroll);
			//监听舞台，这样可以保证拖动滑竿的时候，鼠标在舞台的任意位置松手，都会停止拖动
			scrollBar_sprite.stage.addEventListener(MouseEvent.MOUSE_UP, poleUp);
			//限定拖动范围
			var dragRect : Rectangle = new Rectangle(pole_sprite.x, poleStartY, 0, totalPixels);
			pole_sprite.startDrag(false, dragRect);
			scrollBar_sprite.addEventListener(Event.ENTER_FRAME, poleDown);
			
		}//poleSprite() end
		
		function poleDown(event : Event) : void {
			//在滚动过程中及时获得滑块所处位置
			var nowPosition : Number = Math.floor(pole_sprite.y);
			//使文本随滚动条滚动,这里为什么要加1，可见scroll属性值应该是取正的，也就是说它会删除小数部分，而非采用四舍五入制？
			

			scrollText.scrollV = (scrollText.maxScrollV - 1) * (nowPosition - poleStartY) / totalPixels + 2;
			//误差校正
			var unitPixels : Number = totalPixels / (scrollText.maxScrollV - 1);
			if((nowPosition - poleStartY) < unitPixels) {
				scrollText.scrollV = (scrollText.maxScrollV - 1) * (nowPosition - poleStartY) / totalPixels;
			}
		}//poleDown() end

		function poleUp(event : MouseEvent) : void {
			pole_sprite.stopDrag();
			scrollBar_sprite.removeEventListener(Event.ENTER_FRAME, poleDown);
			scrollBar_sprite.stage.removeEventListener(MouseEvent.MOUSE_UP, poleUp);
			scrollText.addEventListener(Event.SCROLL, textScroll);
			
		}//poleUp() end
		
		
		function bgDown(event : MouseEvent) : void {//--------------/**滑块背景点击
			var nowPosition : Number;
			if((scrollBar_sprite.mouseY - up_btn.y) < (pole_sprite.height / 2)) {
				nowPosition = Math.floor(up_btn.y + up_btn.height);
			}else if((down_btn.y - scrollBar_sprite.mouseY) < pole_sprite.height / 2) {
				nowPosition = Math.floor(down_btn.y - pole_sprite.height);
			}else {
				nowPosition = scrollBar_sprite.mouseY - pole_sprite.height / 2;
			}
			pole_sprite.y = nowPosition;
			scrollText.scrollV = (scrollText.maxScrollV - 1) * (nowPosition - poleStartY) / totalPixels + 2;
			var unitPixels : Number = totalPixels / (scrollText.maxScrollV - 1);
			if((nowPosition - poleStartY) < unitPixels) {
				scrollText.scrollV = (scrollText.maxScrollV - 1) * (nowPosition - poleStartY) / totalPixels + 1;
			}
		}//bgDown() end
		
		
		/**
		 * 下滚动按钮
		 */
		function downBtn(event : MouseEvent) : void {
			scrollText.scrollV++;
			pole_sprite.y = Math.floor(poleStartY + totalPixels * (scrollText.scrollV - 1) / (scrollText.maxScrollV - 1));
			//当鼠标在按钮上按下的时间大于设定时间时，连续滚动
			putTime = getTimer();
			scrollBar_sprite.addEventListener(Event.ENTER_FRAME, downBtnDown);	
		}

		function downBtnDown(event : Event) : void {
			if(getTimer() - putTime > 500) {
				scrollText.scrollV++;
				pole_sprite.y = Math.floor(poleStartY + totalPixels * (scrollText.scrollV - 1) / (scrollText.maxScrollV - 1));
			}
		}	

		function downBtnUp(event : MouseEvent) : void {
			scrollBar_sprite.removeEventListener(Event.ENTER_FRAME, downBtnDown);
		}

		/**
		 * 上滚动按钮
		 */
		function upBtn(event : MouseEvent) : void {
			scrollText.scrollV--;
			pole_sprite.y = Math.floor(poleStartY + totalPixels * (scrollText.scrollV - 1) / (scrollText.maxScrollV - 1));
			//当鼠标在按钮上按下的时间大于设定时间时，连续滚动
			putTime = getTimer();
			scrollBar_sprite.addEventListener(Event.ENTER_FRAME, upBtnDown);
			
		}

		function upBtnDown(event : Event) : void {
			if(getTimer() - putTime > 500) {
				scrollText.scrollV--;
				pole_sprite.y = Math.floor(poleStartY + totalPixels * (scrollText.scrollV - 1) / (scrollText.maxScrollV - 1));
			}
		}

		function upBtnUp(event : MouseEvent) : void {
			scrollBar_sprite.removeEventListener(Event.ENTER_FRAME, upBtnDown);
		}

		/**
		 * 鼠标滚轮事件
		 */
		function mouseWheel(event : MouseEvent) : void {
			if(isSelect == false) {
				scrollText.scrollV -= Math.floor(event.delta / 2);
				
			}else if(isSelect == true) {
				event.delta = 1;
			}
			pole_sprite.y = Math.floor(poleStartY + totalPixels * (scrollText.scrollV - 1) / (scrollText.maxScrollV - 1));
		}
	
		
		
		
		
			
	}//scrollActive() end-----------------------

	
	function scrollOpen():void{
		if (rootA.cpu_power == 2){
			
			scrollOpen_doing();
		}
		if (rootA.cpu_power == 1){
			
			scrollOpen_doing_cpu1();
		}
		
	}
	
	function scrollClose():void{
		if (rootA.cpu_power == 2){
			
			scrollClose_doing();
		}
		if (rootA.cpu_power == 1){
			
			scrollClose_doing_cpu1();
		}
		
		
	}
	
	function scrollActive():void{
		
		scrollActive_doing();
	}
		
	}//class AsColorChange end
	
	
}