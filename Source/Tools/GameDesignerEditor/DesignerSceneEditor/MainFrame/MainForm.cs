using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DevComponents.DotNetBar;
using System.Data.SqlClient;
using System.IO;

namespace DesignerSceneEditor
{
    public partial class MainForm : Form
    {           
        // 选择对象过滤配置窗体
        ObjectSelectMaskForm objectSelectMaskForm = new ObjectSelectMaskForm();

        BaseForm m_ActiveFrm = null;
        public BaseForm ActiveFrm { get { return m_ActiveFrm; } set { m_ActiveFrm = value; } }

        public DevComponents.DotNetBar.SliderItem m_sliderCameraSpeed;

        const int WM_KEYDOWN = 0x0100;
        const int WM_CHAR = 0x0102;

        public bool m_bRenderPoly = true;
        public static MainForm m_MainForm;

        public MainForm()
        {
            InitializeComponent();
            m_sliderCameraSpeed = this.sliderItem1;
            m_MainForm = this;
        }

        public void SetFullScreen(bool bFullScreen)
        {
            this.mainmenu.Visible = !bFullScreen;
            this.barStandard.Visible = !bFullScreen;
            this.barTool.Visible = !bFullScreen;
            this.tabStrip1.Visible = !bFullScreen;
            this.bar1.Visible = !bFullScreen;
            this.FormBorderStyle = bFullScreen ? System.Windows.Forms.FormBorderStyle.None : System.Windows.Forms.FormBorderStyle.Sizable;
            this.WindowState = FormWindowState.Maximized;
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            tabStrip1.MdiForm = this;
            textBoxCameraDis.TextBox.Text = "75000";

            // 初始化状态条内容
            // LHostName.Text = Helper.GetHostName();

            this.buttonOpen.SubItems.Clear();
            LoadHistory();

            EngineLayer.ATLBase.SetShowProfile(0);
        }

        private void LoadHistory()
        {
            string strfile = Path.Combine(Application.StartupPath, "DesignerSceneEditor.history");
            string strHistory = FileFolderHelper.FileToString(strfile);
            if (strHistory.Length == 0) return;

            string[] historys = strHistory.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
            foreach (string history in historys)
            {
                DevComponents.DotNetBar.ButtonItem newitem = new DevComponents.DotNetBar.ButtonItem();
                newitem.ImagePaddingHorizontal = 8;
                newitem.Name = history;
                newitem.Text = history;
                newitem.Click += new System.EventHandler(this.historyItem_Click);

                this.buttonOpen.SubItems.Add(newitem);
            }
        }

        private void historyItem_Click(object sender, EventArgs e)
        {
            DevComponents.DotNetBar.ButtonItem item = sender as DevComponents.DotNetBar.ButtonItem;
            string filename = item.Text;
            OpenDoc(filename);
        }

        private void buttonSave_Click(object sender, EventArgs e)
        {
            if (m_ActiveFrm != null)
            {
                m_ActiveFrm.Save();
            }
        }

        private void buttonOpen_Click(object sender, EventArgs e)
        {
            string filename = FileFolderHelper.BrowseFile(new string[] { "Map" }, Application.StartupPath + "/data/source/maps/");
            if (filename == string.Empty)
                return;

            UpdateHistory(filename);
            OpenDoc(filename);
        }

        private void UpdateHistory(string filename)
        {
            string strfile = Path.Combine(Application.StartupPath, "DesignerSceneEditor.history");
            string strHistory = FileFolderHelper.FileToString(strfile);
            string[] historys = strHistory.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
            if (historys.Length < 4)
            {
                bool bHistoryContainsThisDoc = false;
                foreach (string history in historys)
                {
                    if (history == filename)
                    {
                        bHistoryContainsThisDoc = true;
                        break;
                    }
                }

                if (!bHistoryContainsThisDoc)
                {
                    string strContent = strHistory + (historys.Length == 0 ? string.Empty : "\r\n") + filename;
                    FileFolderHelper.StringToFile(strContent, strfile);
                }
            }
            else
            {
                bool bHistoryContainsThisDoc = false;
                foreach (string history in historys)
                {
                    if (history == filename)
                    {
                        bHistoryContainsThisDoc = true;
                        break;
                    }
                }

                if (!bHistoryContainsThisDoc)
                {
                    string[] newhistorys = new string[] { historys[1], historys[2], historys[3], filename };
                    string content = newhistorys[0] + "\r\n" + newhistorys[1] + "\r\n" + newhistorys[2] + "\r\n" + newhistorys[3];
                    FileFolderHelper.StringToFile(content, strfile);
                }
            }
            
        }

        private void OpenDoc(string filename)
        {
            this.Update();
            EngineLayer.ATLBase.ShowProgressForm(1);
            OpenForm(filename);
            EngineLayer.ATLBase.ShowProgressForm(0);
        }

        public void OpenForm(string filename)
        {
            BaseForm frm = null;
            frm = new BaseForm(filename, this);
            //frm.m_EngineManager = EngineLayer.EngineMgr;
            frm.Text = FileFolderHelper.FullPathToFileName(filename);
            frm.MdiParent = this;
            frm.WindowState = FormWindowState.Maximized;
            // ahpho suntao
            frm.CameraLookAtBox = cameraLocationBox.TextBox;
            frm.CamerSpeedBar = sliderItem1;
            // end ahpho
            frm.Show();
            frm.Update();

            m_ActiveFrm = frm;
        }

        private void dotNetBarManager1_ItemClick(object sender, EventArgs e)
        {
            BaseItem item = sender as BaseItem;
            if (!(ActiveMdiChild is IKDocument))
                return;

            IKDocument activedocument = this.ActiveMdiChild as IKDocument;

            // Activate the form
            if (item.Name == "window_list")
                ((Form)item.Tag).Activate();
            //else if (item == bThemes)
            //    EnableThemes(bThemes);
            //else if (item.GlobalName == bTextColor.GlobalName && activedocument != null)
            //    activedocument.ExecCommand(item.GlobalName, ((ColorPickerDropDown)item).SelectedColor);
            else if (activedocument != null)
            {
                // Pass command to the active document
                // Note the usage of GlobalName property! Since same command can be found on both menus and toolbars, for example Bold
                // you have to give two different names through Name property to these two instances. However, you can and should
                // give them same GlobalName. GlobalName will ensure that properties set on one instance are replicated to all
                // objects with the same GlobalName. You can read more about Global Items feature in help file.
                activedocument.ExecCommand(item.GlobalName, null);
            }
        }

        private void bOptions_Click(object sender, EventArgs e)
        {
            AppOptionsForm fm = new AppOptionsForm();
            if (fm.ShowDialog() == DialogResult.OK)
                fm.Update();
        }

        private void MainForm_MdiChildActivate(object sender, EventArgs e)
        {
//             BaseForm activefrm = (BaseForm)(((MainForm)sender).ActiveMdiChild);
//             BaseForm.K3DEngine.ActivateScene(name);
//             BaseForm.K3DEngine.OnResizeWindow();
        }

        /// <summary>
        /// 摄像机速度改变 ahpho suntao
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void sliderItem1_ValueChanged(object sender, EventArgs e)
        {
            UpdateCameraSpeedToUI();
        }

        public void UpdateCameraSpeedToUI()
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;
            if (baseForm != null)
            {
                float percent = ((float)sliderItem1.Value - sliderItem1.Minimum) / ((float)sliderItem1.Maximum - sliderItem1.Minimum);
                baseForm.SceneSceneEditor.SetCameraSpeed(percent);
            }
        }

        /// <summary>
        /// 修改相机位置 ahpho suntao
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void buttonItem3_Click(object sender, EventArgs e)
        {
            string cameraLocationString = cameraLocationBox.TextBox.Text;

            if (cameraLocationString != "")
            {
                string[] tempArray = cameraLocationString.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                if (tempArray.Length == 3)
                {
                    bool valid = false;
                    int x = 0;
                    int y = 0;
                    int z = 0;

                    valid =  int.TryParse(tempArray[0], out x);
                    
                    if (!valid)
                    {
                        MessageBox.Show("参数类型不正确！", "设置相机位置", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        return;
                    }

                    valid = int.TryParse(tempArray[1], out y);

                    if (!valid)
                    {
                        MessageBox.Show("参数类型不正确！", "设置相机位置", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        return;
                    }

                    valid = int.TryParse(tempArray[2], out z);

                    if (!valid)
                    {
                        MessageBox.Show("参数类型不正确！", "设置相机位置", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        return;
                    }

                    BaseForm baseForm = this.ActiveMdiChild as BaseForm;
                    if (baseForm != null)
                    {
                        baseForm.SetCameraFocus(x, y, z);
                    }
                    else
                    {
                        MessageBox.Show("没有场景被打开！", "设置相机位置", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                }
                else
                {
                    MessageBox.Show("参数必须为三维坐标！", "设置相机位置", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            else
            {
                MessageBox.Show("参数不能为空！", "设置相机位置", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void checkBoxItem1_CheckedChanged(object sender, CheckBoxChangeEventArgs e) // 绘制region边界
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;
            if (baseForm != null)
            {
                baseForm.SetRenderRegionEdge((sender as DevComponents.DotNetBar.CheckBoxItem).Checked);
            }
        }

        private void checkShowPoly_CheckedChanged(object sender, CheckBoxChangeEventArgs e) // 绘制多边形
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;
            if (baseForm != null)
            {
                m_bRenderPoly = checkShowPoly.Checked;
                baseForm.SceneSceneEditor.EnableRenderPoly((sender as DevComponents.DotNetBar.CheckBoxItem).Checked ? 1 : 0);
            }
        }

        private void checkToggleShowProfile_CheckedChanged(object sender, CheckBoxChangeEventArgs e)
        {
            EngineLayer.ATLBase.SetShowProfile(e.NewChecked.Checked ? 1 : 0);
        }

        private void textBoxCameraDis_InputTextChanged(object sender) // camera位置
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;

            if (baseForm != null)
            {
                try
                {
                    float fDis = (float)Convert.ToDouble((sender as DevComponents.DotNetBar.TextBoxItem).TextBox.Text);
                    baseForm.SetCameraDis(fDis);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("输入不正确" + ex.Message);
                    (sender as DevComponents.DotNetBar.TextBoxItem).Text = string.Empty;
                }
            }
        }

        /// <summary>
        /// 退出程序
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bExit_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("确定要退出程序？", "退出程序", MessageBoxButtons.OKCancel, MessageBoxIcon.Question) == DialogResult.OK)
            {
                this.Close();
            }
        }

        /// <summary>
        /// 开启P模式
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bPMode_Click(object sender, EventArgs e)
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;

            if (baseForm != null)
            {
                baseForm.TogglePMode();
            }
        }

        /// <summary>
        /// 开启顶视图
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e"></param>
        private void bTopView_Click(object sender, EventArgs e)
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;

            if (baseForm != null)
            {
                baseForm.ToggleTopView();
            }
        }


        /// <summary>
        /// 选择过滤项
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bFiltSelectObject_Click(object sender, EventArgs e)
        {            
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;

            if (baseForm != null)
            {
                objectSelectMaskForm.ShowDialog();

                if (objectSelectMaskForm.ConfirmSelection)
                {
                    baseForm.SetObjectSelectMask(objectSelectMaskForm.SelectObjectTypeMask);
                }
            }            
        }

        /// <summary>
        /// 复制选中的对象
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bCopy_Click(object sender, EventArgs e)
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;

            if (baseForm != null)
            {
                KeyEventArgs keyEventArgs = new KeyEventArgs(Keys.V);
                baseForm.DoKeyMessage(keyEventArgs);
            }
        }

        /// <summary>
        /// 删除选中的对象
        /// </summary>
        /// <param name="sender">事件发送者</param>
        /// <param name="e">事件参数</param>
        private void bDelete_Click(object sender, EventArgs e)
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;

            if (baseForm != null)
            {
                KeyEventArgs keyEventArgs = new KeyEventArgs(Keys.Delete);
                baseForm.DoKeyMessage(keyEventArgs);
            }
        }

        private void buttonFullScreen_Click(object sender, EventArgs e)
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;
            if (baseForm != null)
            {
                baseForm.ToggleFullScreen();
            }
        }

        private void buttonSelect_Click(object sender, EventArgs e)
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;
            if (baseForm != null)
            {
                KeyEventArgs keyEventArgs = new KeyEventArgs(Keys.D1);
                baseForm.DoKeyMessage(keyEventArgs);
            }
        }

        private void buttonMove_Click(object sender, EventArgs e)
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;
            if (baseForm != null)
            {
                KeyEventArgs keyEventArgs = new KeyEventArgs(Keys.D2);
                baseForm.DoKeyMessage(keyEventArgs);
            }
        }

        private void buttonRotate_Click(object sender, EventArgs e)
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;
            if (baseForm != null)
            {
                KeyEventArgs keyEventArgs = new KeyEventArgs(Keys.D3);
                baseForm.DoKeyMessage(keyEventArgs);
            }
        }

        private void buttonScale_Click(object sender, EventArgs e)
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;
            if (baseForm != null)
            {
                KeyEventArgs keyEventArgs = new KeyEventArgs(Keys.D4);
                baseForm.DoKeyMessage(keyEventArgs);
            }
        }

        private void CreateImageBut_Click(object sender, EventArgs e)
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;
            if (baseForm != null)
            {
                ThumbnailForm form = new ThumbnailForm();
                form.Show(this);
            }
            else
            {
                MessageBox.Show("请先打开场景");
            }
        }

        private void RegionSplitBut_Click(object sender, EventArgs e)
        {
            BaseForm baseForm = this.ActiveMdiChild as BaseForm;
            if (baseForm != null)
            {
                if (baseForm.m_RegionSplitFrom == null)
                {
                    baseForm.m_RegionSplitFrom = new RegionSplitFrom(baseForm.SceneSceneEditor);
                }
                RegionSplitFrom form = baseForm.m_RegionSplitFrom;
                form.Show();
            }
            else
            {
                MessageBox.Show("请先打开场景");
            }
        }

    }
    public interface IKDocument
    {
        void ExecCommand(string strCmd, object data);
    }
}