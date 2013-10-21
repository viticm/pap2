namespace DesignerSceneEditor
{
    partial class TopViewForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.bTabContext = new DevComponents.DotNetBar.ButtonItem();
            this.cmdContextSave = new DevComponents.DotNetBar.ButtonItem();
            this.cmdContextClose = new DevComponents.DotNetBar.ButtonItem();
            this.bTabColor = new DevComponents.DotNetBar.ButtonItem();
            this.cmNodeRightClick = new DevComponents.DotNetBar.ButtonItem();
            this.buttonAddSameLevel = new DevComponents.DotNetBar.ButtonItem();
            this.buttonAddSub = new DevComponents.DotNetBar.ButtonItem();
            this.buttonRename = new DevComponents.DotNetBar.ButtonItem();
            this.buttonCopy = new DevComponents.DotNetBar.ButtonItem();
            this.buttonPaste = new DevComponents.DotNetBar.ButtonItem();
            this.buttonDelete = new DevComponents.DotNetBar.ButtonItem();
            this.cmScriptOp = new DevComponents.DotNetBar.ButtonItem();
            this.bShowLuaCode = new DevComponents.DotNetBar.ButtonItem();
            this.bShowMetainfo = new DevComponents.DotNetBar.ButtonItem();
            this.bEditFile = new DevComponents.DotNetBar.ButtonItem();
            this.bDeleteRecord = new DevComponents.DotNetBar.ButtonItem();
            this.bClearContent = new DevComponents.DotNetBar.ButtonItem();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.miView = new System.Windows.Forms.ToolStripMenuItem();
            this.miTerrain = new System.Windows.Forms.ToolStripMenuItem();
            this.miTerrainLight = new System.Windows.Forms.ToolStripMenuItem();
            this.miTerrainZoneEdge = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.miWater = new System.Windows.Forms.ToolStripMenuItem();
            this.miGrass = new System.Windows.Forms.ToolStripMenuItem();
            this.miFrutex = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.miObjectFar = new System.Windows.Forms.ToolStripMenuItem();
            this.miObjectMedium = new System.Windows.Forms.ToolStripMenuItem();
            this.miObjectNear = new System.Windows.Forms.ToolStripMenuItem();
            this.miObjectPostRender = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.miCamera = new System.Windows.Forms.ToolStripMenuItem();
            this.miRole = new System.Windows.Forms.ToolStripMenuItem();
            this.miCameraAniPoint = new System.Windows.Forms.ToolStripMenuItem();
            this.miQuadTree = new System.Windows.Forms.ToolStripMenuItem();
            this.miOperation = new System.Windows.Forms.ToolStripMenuItem();
            this.miRectSelect = new System.Windows.Forms.ToolStripMenuItem();
            this.miPolygonSelect = new System.Windows.Forms.ToolStripMenuItem();
            this.miBlockPolygon = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.miSelectAllObject = new System.Windows.Forms.ToolStripMenuItem();
            this.miMapManage = new System.Windows.Forms.ToolStripMenuItem();
            this.miLoadNewMap = new System.Windows.Forms.ToolStripMenuItem();
            this.miLoadDefault = new System.Windows.Forms.ToolStripMenuItem();
            this.miObjectManage = new System.Windows.Forms.ToolStripMenuItem();
            this.miTodo = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // bTabContext
            // 
            this.bTabContext.AutoExpandOnClick = true;
            this.bTabContext.GlobalName = "bTabContext";
            this.bTabContext.Name = "bTabContext";
            this.bTabContext.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.cmdContextSave,
            this.cmdContextClose,
            this.bTabColor});
            this.bTabContext.Text = "bTabContext";
            // 
            // cmdContextSave
            // 
            this.cmdContextSave.GlobalItem = false;
            this.cmdContextSave.ImageIndex = 17;
            this.cmdContextSave.Name = "cmdContextSave";
            this.cmdContextSave.Text = "保存";
            this.cmdContextSave.Tooltip = "保存活动文档";
            // 
            // cmdContextClose
            // 
            this.cmdContextClose.Enabled = false;
            this.cmdContextClose.GlobalItem = false;
            this.cmdContextClose.Name = "cmdContextClose";
            this.cmdContextClose.Text = "关闭";
            this.cmdContextClose.Tooltip = "关闭活动文档";
            // 
            // bTabColor
            // 
            this.bTabColor.BeginGroup = true;
            this.bTabColor.GlobalName = "bTabColor";
            this.bTabColor.Name = "bTabColor";
            this.bTabColor.PopupType = DevComponents.DotNetBar.ePopupType.Container;
            this.bTabColor.Text = "更改标签背景色";
            // 
            // cmNodeRightClick
            // 
            this.cmNodeRightClick.AutoExpandOnClick = true;
            this.cmNodeRightClick.Name = "cmNodeRightClick";
            this.cmNodeRightClick.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.buttonAddSameLevel,
            this.buttonAddSub,
            this.buttonRename,
            this.buttonCopy,
            this.buttonPaste,
            this.buttonDelete});
            this.cmNodeRightClick.Text = "cmNodeRightClick";
            // 
            // buttonAddSameLevel
            // 
            this.buttonAddSameLevel.Name = "buttonAddSameLevel";
            this.buttonAddSameLevel.Text = "添加本层";
            // 
            // buttonAddSub
            // 
            this.buttonAddSub.Name = "buttonAddSub";
            this.buttonAddSub.Text = "添加子层";
            // 
            // buttonRename
            // 
            this.buttonRename.BeginGroup = true;
            this.buttonRename.Name = "buttonRename";
            this.buttonRename.Text = "重命名";
            // 
            // buttonCopy
            // 
            this.buttonCopy.Name = "buttonCopy";
            this.buttonCopy.Text = "复制";
            // 
            // buttonPaste
            // 
            this.buttonPaste.Name = "buttonPaste";
            this.buttonPaste.Text = "粘贴";
            // 
            // buttonDelete
            // 
            this.buttonDelete.BeginGroup = true;
            this.buttonDelete.Name = "buttonDelete";
            this.buttonDelete.Text = "删除";
            // 
            // cmScriptOp
            // 
            this.cmScriptOp.AutoExpandOnClick = true;
            this.cmScriptOp.Name = "cmScriptOp";
            this.cmScriptOp.SubItems.AddRange(new DevComponents.DotNetBar.BaseItem[] {
            this.bShowLuaCode,
            this.bShowMetainfo,
            this.bEditFile,
            this.bDeleteRecord,
            this.bClearContent});
            this.cmScriptOp.Text = "cmScriptOp";
            // 
            // bShowLuaCode
            // 
            this.bShowLuaCode.Name = "bShowLuaCode";
            this.bShowLuaCode.Text = "显示代码(&S)";
            // 
            // bShowMetainfo
            // 
            this.bShowMetainfo.BeginGroup = true;
            this.bShowMetainfo.Name = "bShowMetainfo";
            this.bShowMetainfo.Text = "编辑元信息(&X)";
            // 
            // bEditFile
            // 
            this.bEditFile.Name = "bEditFile";
            this.bEditFile.Text = "编辑文件(&E)";
            // 
            // bDeleteRecord
            // 
            this.bDeleteRecord.BeginGroup = true;
            this.bDeleteRecord.Name = "bDeleteRecord";
            this.bDeleteRecord.Text = "删除从表记录(&D)";
            // 
            // bClearContent
            // 
            this.bClearContent.Name = "bClearContent";
            this.bClearContent.Text = "清空";
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miView,
            this.miOperation,
            this.miMapManage,
            this.miObjectManage});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(529, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // miView
            // 
            this.miView.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miTerrain,
            this.miTerrainLight,
            this.miTerrainZoneEdge,
            this.toolStripSeparator2,
            this.miWater,
            this.miGrass,
            this.miFrutex,
            this.toolStripSeparator3,
            this.miObjectFar,
            this.miObjectMedium,
            this.miObjectNear,
            this.miObjectPostRender,
            this.toolStripSeparator4,
            this.miCamera,
            this.miRole,
            this.miCameraAniPoint,
            this.miQuadTree});
            this.miView.Name = "miView";
            this.miView.Size = new System.Drawing.Size(41, 20);
            this.miView.Text = "查看";
            // 
            // miTerrain
            // 
            this.miTerrain.Name = "miTerrain";
            this.miTerrain.Size = new System.Drawing.Size(160, 22);
            this.miTerrain.Text = "地表";
            // 
            // miTerrainLight
            // 
            this.miTerrainLight.Name = "miTerrainLight";
            this.miTerrainLight.Size = new System.Drawing.Size(160, 22);
            this.miTerrainLight.Text = "地表光照";
            // 
            // miTerrainZoneEdge
            // 
            this.miTerrainZoneEdge.Name = "miTerrainZoneEdge";
            this.miTerrainZoneEdge.Size = new System.Drawing.Size(160, 22);
            this.miTerrainZoneEdge.Text = "地表区域边界";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(157, 6);
            // 
            // miWater
            // 
            this.miWater.Name = "miWater";
            this.miWater.Size = new System.Drawing.Size(160, 22);
            this.miWater.Text = "水面";
            // 
            // miGrass
            // 
            this.miGrass.Name = "miGrass";
            this.miGrass.Size = new System.Drawing.Size(160, 22);
            this.miGrass.Text = "草";
            // 
            // miFrutex
            // 
            this.miFrutex.Name = "miFrutex";
            this.miFrutex.Size = new System.Drawing.Size(160, 22);
            this.miFrutex.Text = "灌木";
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(157, 6);
            // 
            // miObjectFar
            // 
            this.miObjectFar.Name = "miObjectFar";
            this.miObjectFar.Size = new System.Drawing.Size(160, 22);
            this.miObjectFar.Text = "物件 - 远距离";
            // 
            // miObjectMedium
            // 
            this.miObjectMedium.Name = "miObjectMedium";
            this.miObjectMedium.Size = new System.Drawing.Size(160, 22);
            this.miObjectMedium.Text = "物件 - 中距离";
            // 
            // miObjectNear
            // 
            this.miObjectNear.Name = "miObjectNear";
            this.miObjectNear.Size = new System.Drawing.Size(160, 22);
            this.miObjectNear.Text = "物件 - 近距离";
            // 
            // miObjectPostRender
            // 
            this.miObjectPostRender.Name = "miObjectPostRender";
            this.miObjectPostRender.Size = new System.Drawing.Size(160, 22);
            this.miObjectPostRender.Text = "物件 - 最后渲染";
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(157, 6);
            // 
            // miCamera
            // 
            this.miCamera.Name = "miCamera";
            this.miCamera.Size = new System.Drawing.Size(160, 22);
            this.miCamera.Text = "主窗口摄像机";
            // 
            // miRole
            // 
            this.miRole.Name = "miRole";
            this.miRole.Size = new System.Drawing.Size(160, 22);
            this.miRole.Text = "模拟主角";
            // 
            // miCameraAniPoint
            // 
            this.miCameraAniPoint.Name = "miCameraAniPoint";
            this.miCameraAniPoint.Size = new System.Drawing.Size(160, 22);
            this.miCameraAniPoint.Text = "摄像机动画点";
            // 
            // miQuadTree
            // 
            this.miQuadTree.Name = "miQuadTree";
            this.miQuadTree.Size = new System.Drawing.Size(160, 22);
            this.miQuadTree.Text = "场景四叉树";
            // 
            // miOperation
            // 
            this.miOperation.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miRectSelect,
            this.miPolygonSelect,
            this.miBlockPolygon,
            this.toolStripSeparator1,
            this.miSelectAllObject});
            this.miOperation.Name = "miOperation";
            this.miOperation.Size = new System.Drawing.Size(41, 20);
            this.miOperation.Text = "操作";
            // 
            // miRectSelect
            // 
            this.miRectSelect.Name = "miRectSelect";
            this.miRectSelect.Size = new System.Drawing.Size(190, 22);
            this.miRectSelect.Text = "矩形选择区域";
            // 
            // miPolygonSelect
            // 
            this.miPolygonSelect.Name = "miPolygonSelect";
            this.miPolygonSelect.Size = new System.Drawing.Size(190, 22);
            this.miPolygonSelect.Text = "多边形区域选择";
            // 
            // miBlockPolygon
            // 
            this.miBlockPolygon.Name = "miBlockPolygon";
            this.miBlockPolygon.Size = new System.Drawing.Size(190, 22);
            this.miBlockPolygon.Text = "障碍物多边形区域选择";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(187, 6);
            // 
            // miSelectAllObject
            // 
            this.miSelectAllObject.Name = "miSelectAllObject";
            this.miSelectAllObject.Size = new System.Drawing.Size(190, 22);
            this.miSelectAllObject.Text = "加载所有物件";
            // 
            // miMapManage
            // 
            this.miMapManage.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miLoadNewMap,
            this.miLoadDefault});
            this.miMapManage.Name = "miMapManage";
            this.miMapManage.Size = new System.Drawing.Size(65, 20);
            this.miMapManage.Text = "地图管理";
            // 
            // miLoadNewMap
            // 
            this.miLoadNewMap.Name = "miLoadNewMap";
            this.miLoadNewMap.Size = new System.Drawing.Size(202, 22);
            this.miLoadNewMap.Text = "加载一张新地图到该场景";
            // 
            // miLoadDefault
            // 
            this.miLoadDefault.Name = "miLoadDefault";
            this.miLoadDefault.Size = new System.Drawing.Size(202, 22);
            this.miLoadDefault.Text = "加载默认地形";
            // 
            // miObjectManage
            // 
            this.miObjectManage.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.miTodo});
            this.miObjectManage.Name = "miObjectManage";
            this.miObjectManage.Size = new System.Drawing.Size(65, 20);
            this.miObjectManage.Text = "物件管理";
            // 
            // miTodo
            // 
            this.miTodo.Name = "miTodo";
            this.miTodo.Size = new System.Drawing.Size(118, 22);
            this.miTodo.Text = "啥都没有";
            // 
            // TopViewForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(529, 492);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "TopViewForm";
            this.Text = "顶视图";
            this.TopMost = true;
            this.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.TopViewForm_MouseDoubleClick);
            this.Resize += new System.EventHandler(this.TopViewForm_Resize);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.TopViewForm_MouseUp);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.TopViewForm_FormClosing);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.TopViewForm_MouseMove);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.TopViewForm_MouseDown);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private DevComponents.DotNetBar.ButtonItem bTabContext;
        private DevComponents.DotNetBar.ButtonItem cmdContextSave;
        private DevComponents.DotNetBar.ButtonItem cmdContextClose;
        private DevComponents.DotNetBar.ButtonItem bTabColor;
        private DevComponents.DotNetBar.ButtonItem cmNodeRightClick;
        private DevComponents.DotNetBar.ButtonItem buttonAddSameLevel;
        private DevComponents.DotNetBar.ButtonItem buttonAddSub;
        private DevComponents.DotNetBar.ButtonItem buttonRename;
        private DevComponents.DotNetBar.ButtonItem buttonCopy;
        private DevComponents.DotNetBar.ButtonItem buttonPaste;
        private DevComponents.DotNetBar.ButtonItem buttonDelete;
        private DevComponents.DotNetBar.ButtonItem cmScriptOp;
        private DevComponents.DotNetBar.ButtonItem bShowLuaCode;
        private DevComponents.DotNetBar.ButtonItem bShowMetainfo;
        private DevComponents.DotNetBar.ButtonItem bEditFile;
        private DevComponents.DotNetBar.ButtonItem bDeleteRecord;
        private DevComponents.DotNetBar.ButtonItem bClearContent;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem miView;
        private System.Windows.Forms.ToolStripMenuItem miTerrain;
        private System.Windows.Forms.ToolStripMenuItem miTerrainLight;
        private System.Windows.Forms.ToolStripMenuItem miTerrainZoneEdge;
        private System.Windows.Forms.ToolStripMenuItem miOperation;
        private System.Windows.Forms.ToolStripMenuItem miMapManage;
        private System.Windows.Forms.ToolStripMenuItem miObjectManage;
        private System.Windows.Forms.ToolStripMenuItem miWater;
        private System.Windows.Forms.ToolStripMenuItem miGrass;
        private System.Windows.Forms.ToolStripMenuItem miFrutex;
        private System.Windows.Forms.ToolStripMenuItem miObjectFar;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripMenuItem miObjectMedium;
        private System.Windows.Forms.ToolStripMenuItem miObjectNear;
        private System.Windows.Forms.ToolStripMenuItem miObjectPostRender;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem miCamera;
        private System.Windows.Forms.ToolStripMenuItem miRole;
        private System.Windows.Forms.ToolStripMenuItem miCameraAniPoint;
        private System.Windows.Forms.ToolStripMenuItem miQuadTree;
        private System.Windows.Forms.ToolStripMenuItem miRectSelect;
        private System.Windows.Forms.ToolStripMenuItem miPolygonSelect;
        private System.Windows.Forms.ToolStripMenuItem miBlockPolygon;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem miSelectAllObject;
        private System.Windows.Forms.ToolStripMenuItem miLoadNewMap;
        private System.Windows.Forms.ToolStripMenuItem miLoadDefault;
        private System.Windows.Forms.ToolStripMenuItem miTodo;
    }
}