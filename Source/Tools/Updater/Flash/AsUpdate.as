package {

	import flash.display.Sprite;
	import flash.display.MovieClip;
	import flash.text.TextField;
	import flash.external.ExternalInterface;
	import flash.events.*;
	import flash.display.SimpleButton;
	import flash.net.URLRequest;
	import flash.net.URLLoader;
	
	import As_back;

	import As_btn;
	import As_ad;
	import As_log;
	import As_scroll;

	public class AsUpdate extends Sprite {

		//为父类初始化，开辟内存池
		private const As_back_in : As_back = new As_back();
		private const As_btn_in : As_btn = new As_btn();
		private const As_ad_in : As_ad = new As_ad();
		private const As_log_in : As_log = new As_log();
		private const As_scroll_in : As_scroll = new As_scroll();

		//3个外部xml地址
		public var url_log:String = "";
		public var url_xml1:String = "";
		public var url_xml2:String = "";

		//3个主按钮 退出游戏 设置  开始游戏的状态开关，默认开启
		public var btn_use_quit:Boolean = true;
		public var btn_use_config:Boolean = true;
		public var btn_use_startGame:Boolean = true;
		
		//对外全局控制参数
		public var cpu_power:uint = 1;//cpu的性能判断，默认1单核，2双核及其以上
		
		public var URL_config:String = "no.xml";//config动态地址
		
		public var todayDate_root:uint;//定义一个时间星期值
		
		public var txt_version_content:String = " ";
		
		public var logcantOpen:Boolean = true;//log卷轴是否可以开启开关
		
		public var log_opened:Boolean = true;//log卷轴是否已经打开的标示
		
		public var xml_data_log:XML = <xml></xml>;//log更新日志文件内容
		
		public function AsUpdate():void {//----------------------------------------------------------------------*//* Class AsUpdate构造函数
			
			//把其他类的路径控制符指向这里，这样其他类就可以通过该符号访问这个核心类
			
			As_back_in.rootA = this;//管理背景图片变色
			As_btn_in.rootA = this;//大部分按钮(卷轴内部两个关闭退出按钮除外)控制类
			As_ad_in.rootA = this;//ad1(广告部分)外部网页内容连接
			
			As_log_in.rootA = this;//滚动log卷轴打开与关闭的控制
			As_scroll_in.rootA = this;//卷轴内滚动条与文字内容的控制

			_load_self();//导入flash自身，确保swf载入完成，和后台同步
			
		}//------------------------------------------------------------------------------------------------------*//* Class AsUpdate构造函数
		
		//--------------------------** 处理ad1内部的幻灯hd
		function As_ad1_hdPlay() {

			As_ad_in.hd_play();
		}
		function As_ad1_hdStop() {

			As_ad_in.hd_stop();
		}
		//--------------------- ** 加入ad1内部的10条网页信息
		function As_webTxt_add() {

			_ad1.addChild(As_ad_in);

		}
		function As_removeLightTride() {

			_ad1.removeChild(As_ad_in);

		}
		
		//--------------------------** 处理日志滚动条、文本 开始
		function As_scrollActive(){
			
			As_scroll_in.scrollActive();
			
		}

		function As_scrollOpen() {

			As_scroll_in.scrollOpen();

		}
		function As_scrollClose() {

			As_scroll_in.scrollClose();

		}
		//--------------------------** 处理日志滚动条、文本 结束

		//--------------------------** 处理log卷轴 开始

		function As_logActive() {//激活log卷轴
			
			As_log_in.logActive();

		}
		function As_log_btnActive() {//激活log卷轴内的两个关闭按钮

			As_log_in.log_btnActive_cool();

		}
		function As_logClose() {//关闭卷轴窗口

			As_log_in.log_backClose();

		}
		function As_logErro(){
			As_log_in.log_erro_active();
		}
		//--------------------------** 处理log卷轴 结束


		//-------------------------------------------------------------------*** callBack 开始
		
		//下载进度条运行
		private function callBack_ProgressPercent_doing(Param:String) {

			var int_Percent_of_update:uint = uint(Param);

			_planRound_main.gotoAndStop(int_Percent_of_update);//主进度条
			
			_planRound_txt.gotoAndStop(int_Percent_of_update);//进度条走动百分比显示

			txt_downLoad.text = "更新包下载完成  " + Param + "%";//文字显示更新信息和百分比

			_planRound_txt.txt_back.txt.text = Param + "%";//文字显示百分比 随进度条走动的

		}//callBack_ProgressPercent_doing() end
		
		//下载进度条激活
		private function callBack_ProgressPercent():void {

			ExternalInterface.addCallback("ProgressPercent",callBack_ProgressPercent_doing);//得到recv回调用callBack_ProgressPercent_doing()
			
			txt_downLoad.text = "正在进行更新前初始化，请等待...";
			
		}

		//3个主按钮激活
		private function btn3_active(){
			
			_btnMain.btn_startGame.gotoAndStop("over_com");
			
			_dragonRound.gotoAndPlay("round_little_forever");
			
		}
		
		private function callBack_SetStatus_doing(Param:String) {
			
			var Status_of_update:uint;

			Status_of_update = uint(Param);//更新状态赋值
			
			if (Status_of_update == 101) {//正在自身升级,所有按钮冷却

				txt_downLoad.text = "正在准备更新...";//文字显示更新信息和百分比

				txt_downLoad.textColor = 0xfdf788;
				
				btn_use_quit = false;
				
				_btnMain.back_closeWindow.gotoAndStop("noup");
				
				_btnMain.btn_quit.gotoAndStop("noup");

				btn_use_startGame = false;
				_btnMain.btn_startGame.gotoAndStop("noup");

			}
			if (Status_of_update == 102) {//正在下载更新包（仅退出，设置2个按钮可用，这个时候需要访问网络去下载这次更新的补丁说明
				
				txt_downLoad.text = "正在下载更新包...";//文字显示更新信息和百分比
				
				btn_use_startGame = false;
				
				_btnMain.btn_startGame.gotoAndStop("downloading");
				
				btn_use_quit = true;
				
				_btnMain.back_closeWindow.gotoAndPlay("up");
				
				_btnMain.btn_quit.gotoAndPlay("up");
				
					stop_all();
					
			}
			if (Status_of_update == 103) {//正在更新客户端程序（仅设置按钮可用）
		
				As_btn_in.btnAd2Click();//调用打开log
				
				As_log_in.speed_logOpen = 5;
				
				As_log_in.croll_log = 422.4;
				
				txt_downLoad.text = "正在更新客户端程序...";//文字显示更新信息和百分比

				txt_downLoad.textColor = 0xfdf788;
				
				_planRound_txt.txt_back.txt.text = " ";//文字显示百分比 随进度条走动的

				_planRound_main.gotoAndStop(100);//主进度条
				
				btn_use_startGame = false;
				
				_btnMain.btn_startGame.gotoAndStop("installing1");
				
				_btnMain.back_closeWindow.gotoAndStop(1);

				_btnMain.btn_quit.gotoAndStop(1);

				btn_use_quit = false;
				
				
				
				if (cpu_power == 2) {
					
					_install_w.gotoAndStop(2);
					
				}

			}
			if (Status_of_update == 104) {//客户端更新完毕（全部按钮激活）
			
				txt_downLoad.text = "客户端更新完毕";//文字显示更新信息

				_planRound_txt.txt_back.txt.text = "";//文字百分比 随进度条走动的

				_planRound_main.gotoAndStop(100);//主进度条
				
				btn_use_startGame = true;
				_btnMain.btn_startGame.gotoAndPlay("up");
				
				_versionBg.gotoAndPlay("over");//版本号闪光装饰
				
				btn_use_quit = true;
				
				_btnMain.back_closeWindow.gotoAndPlay(2);
				
				_btnMain.btn_quit.gotoAndPlay(2);

				
				if (cpu_power == 2) {
					
					_dragonRound.gotoAndPlay("c1");//下载后更新客户端完成后，右边圆圈光线(_dragonRound)改变转动轨迹
			
					_dragonRound.addFrameScript(397,btn3_active);
			
					
					_install_w.gotoAndStop(1);
					
				}
				

			}
			if (Status_of_update == 105) {//最新版本，无需更新（全部按钮激活）
			
				txt_downLoad.text = "客户端已是最新版本，无需更新";//文字显示更新信息
				
				_planRound_main.gotoAndStop(100);//主进度条
				
				_versionBg.gotoAndPlay("over");//版本号闪光装饰
				
				btn_use_quit = true;
				
				_btnMain.back_closeWindow.gotoAndPlay("up");
				
				_btnMain.btn_quit.gotoAndPlay("up");

				btn_use_startGame = true;
				_btnMain.btn_startGame.gotoAndPlay("up");

			}
		}//callBack_SetStatus_doing() end
		private function callBack_SetStatus() {

			ExternalInterface.addCallback("SetStatus",callBack_SetStatus_doing);//得到recv回调用callBack_SetStatus_doing()
			
		}
		
		//执行错误输出判断
		private function callBack_SetError_doing(Param:String) {

			var Error_of_update:uint;

			Error_of_update = uint(Param);//错误信息赋值
			
			if (Error_of_update == 200) {
				return;
			}

			if (Error_of_update == 201) {

				txt_downLoad.text = "缺少必要的启动文件，启动失败！";
				
				txt_downLoad.textColor = 0xff7875;
				
			}
			if (Error_of_update == 202) {

				txt_downLoad.text = "连接服务器超时，请检查网络！";

			}
			if (Error_of_update == 203) {

				txt_downLoad.text = "网络中断，请检查网络连接！";
				
			}
			if (Error_of_update == 204) {

				txt_downLoad.text = "未找到文件，服务器可能无此更新包文件";

			}
			if (Error_of_update == 205) {

				txt_downLoad.text = "磁盘空间不足!";

			}
			if (Error_of_update == 206) {

				txt_downLoad.text = "写文件出错!";

			}
			if (Error_of_update == 207) {

				txt_downLoad.text = "无可用更新包，需手动更新";

			}
			if (Error_of_update == 208) {

				txt_downLoad.text = "更新失败！";

			}
			if (Error_of_update == 209) {

				txt_downLoad.text = "手动更新包不适合当前版本";

			}
			
			txt_downLoad.textColor = 0xffffff;
			
			if (todayDate_root == 4){
				
				txt_downLoad.textColor = 0xff7875;//星期2字体粉红色
			}
			
			btn_use_quit = true;
			_btnMain.btn_quit.gotoAndPlay("up");

			btn_use_config = true;
			_btnMain.btn_config.gotoAndPlay("up");

			btn_use_startGame = true;
			_btnMain.btn_startGame.gotoAndPlay("up");


		}//callBack_SetError_doing end

		private function callBack_SetError() {

			ExternalInterface.addCallback("SetError",callBack_SetError_doing);//得到recv回调用callBack_SetError_doing()
			
		}//callBack_SetError() end



		private function callBack_SetVersion_doing(Param:String) {
			
			txt_version.text = "当前版本:"+Param;//文字显示版本信息
			
			txt_version_content = "当前版本:"+Param;
			
			if (todayDate_root == 2){
					
					txt_version.textColor = 0xffffff;//星期2字体白色
			}
			if (todayDate_root == 7 ||todayDate_root == 0 ){
					
					txt_version.textColor = 0xccccff;
			}
			if (todayDate_root == 6){
					
					txt_version.textColor = 0xffcccc;
			}
			if (todayDate_root == 5){
					
					txt_version.textColor = 0xa8cece;
			}
			if (todayDate_root == 3){
					
					txt_version.textColor = 0xffccff;
			}
			if (todayDate_root == 1){
					
					txt_version.textColor = 0x99ccff;
			}
			
		}
		private function callBack_SetVersion():void {

				ExternalInterface.addCallback("SetVersion",callBack_SetVersion_doing);//得到recv回调用callBack_SetError_doing()
				
		}
		//-------------------------------------------------------------------------------

		private function callBack_infoControl_doing(Param:String) {
		
			var info_array:Array = Param.split("&");
			
			cpu_power = uint(info_array[0]);
			
			URL_config = String(info_array[2]);
			
			if (info_array[1] == "16"){
				
				todayDate_root = 8;
			}
			
			if (cpu_power > 1){
				
				cpu_power = 2;
			}
			
			As_btn_in.btnActive();//激活所有按钮，
			
			_btnMain.btn_quit.gotoAndPlay("up");
				
			_btnMain.btn_config.gotoAndPlay("up");

			_btnMain.btn_startGame.gotoAndPlay("up");
			
			_btnMain.back_closeWindow.gotoAndPlay("up");
			
			config_load();
			
			if (cpu_power == 2) {

				play_all();

			} else if (cpu_power == 1) {

				stop_all();

				_btnMain.btn_startGame.shanjiao.gotoAndStop(1);
			}
			
			info_array = null;
		}

		private function callBack_infoControl() {
			
			ExternalInterface.addCallback("SetComputerInfo",callBack_infoControl_doing);//得到recv回调用callBack_cpuControl_doing()
			
		}
		//-------------------------------------------------------------------*** callBack 结束
		
		//拖动控制
		
		public function btn_ad1_2_ad1(){
			
			As_btn_in.adMain = "ad1";
			
			if (cpu_power == 2){
				
				_btnMain.back_ad1_ad2.gotoAndPlay("toad1");
			
			}else{
				
				_btnMain.back_ad1_ad2.gotoAndStop(1);
			}
			
		}
		public function drag_scrollOn() {

			_dragBack.gotoAndStop(2);

		}
		public function drag_scrollOff() {
			
			if (cpu_power == 2){
				
				play_all();
				
				btn_ad1_2_ad1();
			}
			_dragBack.gotoAndStop(1);

		}
		

		//-----------------------------------------------------------------*** call集合 开始

		public function call_Startgame() {

			ExternalInterface.call("Startgame");//初始化发送Startgame

		}
		public function call_Configgame() {

			ExternalInterface.call("Configgame");//初始化发送Configgame

		}
		public function call_Quitupdate() {

			ExternalInterface.call("CloseWindow");//初始化发送Quitupdate
			
		}
		public function call_MinWindow() {

			ExternalInterface.call("MinWindow");//初始化发送MinWindow

		}
		
		private function call_dragMoveAll(e:MouseEvent) {

			ExternalInterface.call("CanMove");

		}
		//-------------------------------------------------------------------*** call集合 结束
		
		//关闭 运行动态效果，
		public function stop_all(){
			
			_dragonRound.stop();
					
			guangdian.stop();
			
		}
		
		public function play_all(){
			
			_dragonRound.play();
					
			guangdian.play();
			
		}
		
		//键盘事件
		
		//载入config_updater这个xml配置文件，总体载入第3步！！！
		private function config_load(){
			
			import flash.utils.getTimer;

			var nowday:Date = new Date();
			
			var now:String = "?"+ String(nowday.seconds);
			
			var xml_loader:URLLoader = new URLLoader(new URLRequest(URL_config+now));
			
			function config_loading(event:ProgressEvent) {
				
				_ad1.erro_web.txt.text = "下载配置文件，已完成  "+ Math.round ((event.bytesLoaded/event.bytesLoaded)*100)+"%，数据处理中...";
				
			}
			function config_loadError(event:IOErrorEvent) {
				
				logcantOpen = false;
				
				_ad1.erro_web.txt.text = "下载配置文件失败！\n建议检查您的网络，或使用有效的方式与我们的客服联系。\n注：此功能不影响您开始游戏 ";
				
				As_back_in.backActive();//激活背景，并开始变色
				
				Gc_this();
				
			}
			function config_loaded(event:Event) {
				
				_ad1.erro_web.txt.text = " ";
				
				var xml_data:XML = XML(xml_loader.data);
				
				var today_backColor:uint;//外部xml主动控制updater变色值，0为系统自动变色，有效范围1~7;
				
				url_log = String(xml_data.child("configswf")[0])+now ;//加入一个时间值，保证每次连接不一样，不会重复读取IE缓存

				url_xml1 = String(xml_data.child("configswf")[1])+now;

				url_xml2 = String(xml_data.child("configswf")[2])+now;
				
				today_backColor = uint(xml_data.child("configswf")[3]);
				
				if (today_backColor > 0){//酒红色
				
					todayDate_root = today_backColor;
				
					As_back_in.backActive();//激活背景，并开始变色
				}
				
				if (todayDate_root == 8){//16位
				
					todayDate_root = 8;
				
					As_back_in.backActive();//激活背景，并开始变色
				}
				
				logcantOpen = true;
				
				As_ad_in.adActive();//激活左右切换的ad公告和日志部分
				
				//释放资源
				Gc_this();
				
				now = null;
				
			}
			
			function Gc_this(){
				
				xml_loader.removeEventListener(Event.COMPLETE, config_loaded);
				xml_loader.removeEventListener(ProgressEvent.PROGRESS, config_loading);
				xml_loader.removeEventListener(IOErrorEvent.IO_ERROR, config_loadError);
				
				xml_loader = null;
				
			}
			xml_loader.addEventListener(Event.COMPLETE, config_loaded);
			xml_loader.addEventListener(ProgressEvent.PROGRESS, config_loading);
			xml_loader.addEventListener(IOErrorEvent.IO_ERROR, config_loadError);

		}//config_load() end

		//load自身!!!!!!!总体载入第1步
		private function _load_self() {

			loaderInfo.addEventListener(Event.COMPLETE, loadComplete);

			function loadComplete(event:Event):void {
				
				
				callBack_SetStatus();

				callBack_ProgressPercent();

				callBack_SetError();

				callBack_SetVersion();

				_dragBack.addEventListener(MouseEvent.MOUSE_DOWN,call_dragMoveAll);//全局拖动侦听

				todayDate_root = new Date().getUTCDay();//获取系统日期时间
				
				As_back_in.backActive();//先自动变色
				
				callBack_infoControl();//全局导入口，必须先判断CPU性能！
				
				loaderInfo.removeEventListener(Event.COMPLETE, loadComplete);//释放资源
				
			}

		}//_load_self() end


	}//class AsUpdate end

}