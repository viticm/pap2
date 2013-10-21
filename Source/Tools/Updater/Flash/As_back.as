package{
	
	import flash.display.MovieClip;
	import flash.display.Sprite;
	import flash.filters.ColorMatrixFilter;

	public class As_back extends Sprite
	
	{
		
		public var rootA:Object;//主舞台根路径
		
		public function As_back(){
			
		}
		
		private function backActive_doing():void{
			
			const back_main:MovieClip = rootA._backMain;
			
			const btn_back:MovieClip = rootA._btnBack;
			
			const hd_back:MovieClip = rootA._ad1.slide.hd_back;
			
			const btn_jiantou_up:MovieClip = rootA._btnMain.btn_ad1Up.btn_jiantou;
			const btn_jiantou_down:MovieClip = rootA._btnMain.btn_ad1Down.btn_jiantou;
			
			const roundBack:MovieClip = rootA._dragonRound.roundBack;
			
			const juan_up:MovieClip = rootA._log.juanUp.juanBack;
			const juan_down:MovieClip = rootA._log.juanDown.juanBack;
			
			const back_log_back:MovieClip = rootA._log.juanMiddle.juan_back_n;
			
			//----------------------------------*** 变色 1为蓝色，2为黄色，3为粉红色，4为黑色，5为墨绿色，6红色，7紫色；
			
			var todayDate:uint;//定义一个时间星期值
		
			todayDate = rootA.todayDate_root;
			
			if (todayDate == 0){//紫色
				
				todayDate = 7;
				
			}
				
			
			back_main.gotoAndStop(todayDate + 1);
			
			if (todayDate == 8){
				
				todayDate = 4;
				
			}
			
			btn_back.gotoAndStop(todayDate);
			btn_jiantou_up.gotoAndStop(todayDate);//ad1的上下控制按钮
			hd_back.gotoAndStop(todayDate);//幻灯内部的123按钮背景
			
			roundBack.gotoAndStop(todayDate);
			
			btn_jiantou_down.gotoAndStop(todayDate);
			juan_up.gotoAndStop(todayDate);
			juan_down.gotoAndStop(todayDate);
			back_log_back.gotoAndStop(todayDate);
			

		}//backActive_doing() end
		
		function backActive(){
			
			backActive_doing();
		}
		
	}//class AsColorChange end
	
	
}