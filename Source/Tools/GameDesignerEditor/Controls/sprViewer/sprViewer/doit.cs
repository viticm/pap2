using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Xml;
using System.Collections;
using System.IO;
using System.Drawing;
using System.Reflection;
using System.Data.SqlClient;
using System.Data;
using System.Drawing.Imaging;

using atlaxLib;

namespace sprViewer
{
    public class doit
    {
        private string strFileName = "";        
        private frmTest ft;

        /// <summary>
        /// 初始化
        /// </summary>
        /// <returns>Control的引用</returns>
        public Control Init()
        {
            ft = new frmTest();
            return ft.panel1;
        }

        // 又存在没有初始化的情况
        public void InitCacae(SqlConnection conn)
        {
            Helper.GetHelper().CurrentSqlConnection = conn;
            Helper.GetHelper().InitCacheTable();
        }

        public void UnInit()
        {            
            ft.timer1.Enabled = false;
            ft.Close();            
        }

        /// <summary>
        /// 设置spr文件名，会自动出现预览界面
        /// </summary>
        /// <param name="strFileName">SPR文件名，包含全路经</param>
        public void SetFileName(string strFileName, Test3DEngine engine)
        {
            if (strFileName.LastIndexOf('.') == -1)
            {
                throw (new Exception("传入文件名不合法"));
            }

            string strExt = strFileName.Substring(strFileName.LastIndexOf('.') + 1).ToLower();
            ft.timer1.Enabled = false;
            ft.labError.Visible = false;
            this.strFileName = strFileName;

            if(!File.Exists(strFileName))
            {
                ft.labError.Visible = true;
                ft.pb1.Image = new Bitmap(10, 10);
                return;
            }

            SPRItem si;
            if(strExt == "mdl")
            {
                si = new SPRItem(strFileName, SPRItem.PIC_TYPE.MDL, engine);
            }
            else if(strExt == "mtl")
            {
                si = new SPRItem(strFileName, SPRItem.PIC_TYPE.MTL, engine);
            }
            else if (strExt == "mesh")
            {
                si = new SPRItem(strFileName, SPRItem.PIC_TYPE.MESH, engine);
            }
            else if (strExt == "ani")
            {
                si = new SPRItem(strFileName, SPRItem.PIC_TYPE.ANI, engine);
            }
            else if (strExt == "tani")
            {
                si = new SPRItem(strFileName, SPRItem.PIC_TYPE.ANI, engine);
            }
            else //spr
            {
                si = new SPRItem(strFileName);
            }

            ft.sio = si;
            ft.timer1.Enabled = true;
        }

        /// <summary>
        /// 设置UITex文件名
        /// </summary>
        /// <param name="strFileName"></param>
        /// <param name="nFrame">帧数</param>
        public void SetUtexFileName(string strFileName, int nFrame)
        {
            ft.timer1.Enabled = false;
            ft.labError.Visible = false;
            this.strFileName = strFileName;
            SPRItem si;

            if (!File.Exists(strFileName))
            {
                ft.labError.Visible = true;
                ft.pb1.Image = new Bitmap(10, 10);
                return;
            }

            si = new SPRItem(strFileName, nFrame);
            ft.sio = si;
            ft.timer1.Enabled = true;
        }
    
        public void SetItemInfo(
            string iName, int iColor, bool iCanStack, int iMax,             //道具名称
            int tGenre, int tBindType,                                      //绑定类型
            int maxExist,                                                   //最大拥有
            string desc,                                                    //说明
            bool canTrade, int price,                                       //价格
            string icoFileName, int icoFrame,                                //图标
            int maxexisttime
            )
        {
            ft.timer1.Enabled = false;

            //映射表 for iColor
            Brush[] colorTable = { 
                new SolidBrush(Color.FromArgb(159, 159, 159)),    //灰
                Brushes.White,                                  //白
                new SolidBrush(Color.FromArgb(20, 255, 20)),    //绿
                new SolidBrush(Color.FromArgb(32, 160, 255)),   //蓝
                new SolidBrush(Color.FromArgb(150, 0, 255)),  //紫
                new SolidBrush(Color.FromArgb(250, 150, 0)),  //橙
                new SolidBrush(Color.FromArgb(0xB8, 0x86, 0x0B)), //金
                };
            //映射表 for tGenre 道具主类
            string[] genreTable = { "装备", "药品", "任务道具", "材料" ,"阅读", "称号", "镶嵌"};
            Font[] fontTable = { new Font("宋体", 11, FontStyle.Bold), new Font("宋体", 9) };

            //行距
            int nRowLen = 18;

            //解释desc先
            string strDesc1 = "";
            string strDesc2 = "";
            {
                string[] asp = { "<Text>text=\"", "\" font=", "</text>", @"\\\n" };
                string[] sr = desc.Replace("<text>","<Text>").Split(asp, StringSplitOptions.RemoveEmptyEntries);
                if (sr.Length == 4)
                {
                    strDesc1 = sr[0];
                    strDesc2 = sr[2];
                }
                else if (sr.Length == 2)
                {
                    if (sr[0].StartsWith("使用：")
                        || sr[0].StartsWith("装备：")
                        || sr[0].StartsWith("携带：")
                        )
                    {
                        strDesc1 = sr[0];
                    }
                    else
                    {
                        strDesc2 = sr[0];
                    }
                }
            }

            //宽度
            int nWidth = 0;
            {
                Image img1 = new Bitmap(500, 500);
                Graphics g1 = Graphics.FromImage(img1);
                nWidth = 220;
                int i = 0;
                i = (int)g1.MeasureString(strDesc1, fontTable[1]).Width;
                nWidth = nWidth < i ? i : nWidth;
                i = (int)g1.MeasureString(strDesc2, fontTable[1]).Width;
                nWidth = nWidth < i ? i : nWidth;
                nWidth += 30;
                img1 = null;
                g1 = null;
            }

            //高度
            int nHeight = 0;
            {
                nHeight = nRowLen * 2 + 5;
                if (!(tGenre != 2 && tBindType < 2)) nHeight += nRowLen;
                if (maxExist >= 1) nHeight += nRowLen;
                if (canTrade) nHeight += nRowLen;
                nHeight += ((desc.Length - desc.ToLower().Replace("<text>", "").Length) / 6) * nRowLen;
                if (maxexisttime != 0) nHeight += nRowLen;
                string[] descs = desc.Split(new string[] {"\\n"}, StringSplitOptions.RemoveEmptyEntries);
                nHeight += (descs.Length - 1) * nRowLen;
            }

            //读图标
            Image imgico;
            try
            {
                FileInfo fi = new FileInfo(icoFileName);
                Image imgico1 = tga.uitex2img(fi.DirectoryName, icoFileName, icoFrame);
                Bitmap b = new Bitmap(imgico1.GetThumbnailImage(48, 48, null, IntPtr.Zero));
                b.SetResolution(96, 96);
                imgico = b;
                nWidth += imgico.Width;
            }
            catch
            {
                imgico = null;
            }

            //光标高度
            int nPosy = 5;
            Image img = new Bitmap(nWidth, nHeight);
            Graphics g = Graphics.FromImage(img);
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
            g.TextRenderingHint = System.Drawing.Text.TextRenderingHint.ClearTypeGridFit;
            //开始画
            {
                //画背景
                g.FillRectangle(new SolidBrush(Color.FromArgb(0x29, 0x29, 0x29)), new Rectangle(0, 0, nWidth, nHeight));
                g.DrawRectangle(Pens.White, new Rectangle(0, 0, nWidth - 1, nHeight - 1));

                //画图标
                if (imgico != null)
                    g.DrawImage(imgico, new Point(nWidth - imgico.Width - 5, 5));
                else
                    g.DrawString("无法显示", new Font("宋体", 9), Brushes.White, new Point(nWidth - 65, 5));


                //画文字
                {
                    //画名称
                    string strPaint = iName;
                    g.DrawString(strPaint, fontTable[0], colorTable[iColor], 5, nPosy);
                    int w = (int)g.MeasureString(strPaint, fontTable[0]).Width;
                    strPaint = "";
                    if (iCanStack && iMax > 1)
                    {
                        strPaint = "(" + iMax.ToString() + ")";
                    }
                    if (!iCanStack)
                    {
                        strPaint = "(" + iMax.ToString() + "次)";
                    }
                    g.DrawString(strPaint, fontTable[1], Brushes.White, w + 5, nPosy + 2);
                    nPosy += nRowLen;
                    strPaint = "";

                    //画绑定类型
                    if (tGenre == 2)
                    {
                        strPaint = "拾取后绑定";
                    }
                    else
                    {
                        if (tBindType == 2) strPaint = "使用后绑定";
                        if (tBindType == 3) strPaint = "拾取后绑定";
                    }
                    if (strPaint != "")
                    {
                        g.DrawString(strPaint, fontTable[1], Brushes.White, 12, nPosy);
                        nPosy += nRowLen;
                    }
                    strPaint = "";

                    //画唯一属性
                    if (maxExist == 1) strPaint = "唯一";
                    if (maxExist > 1) strPaint = "唯一(" + maxExist + ")";
                    if (strPaint != "")
                    {
                        g.DrawString(strPaint, fontTable[1], Brushes.White, 12, nPosy);
                        nPosy += nRowLen;
                    }

                    //画道具类型
                    strPaint = genreTable[tGenre];
                    {
                        g.DrawString(strPaint, fontTable[1], Brushes.White, 12, nPosy);
                        nPosy += nRowLen;
                    }
                    strPaint = "";

                    //画道具说明1
                    strPaint = strDesc1;
                    if (strPaint != "")
                    {
                        g.DrawString(strPaint, fontTable[1], new SolidBrush(Color.FromArgb(20, 255, 20)), 12, nPosy);
                        nPosy += nRowLen;
                    }
                    strPaint = "";

                    //画道具说明2
                    strPaint = strDesc2;
                    if (strPaint != "")
                    {
                        string[] strPaints = strPaint.Split(new string[] { "\\n" }, StringSplitOptions.RemoveEmptyEntries);
                        foreach (string _s in strPaints)
                        {
                            g.DrawString(_s, fontTable[1], Brushes.Yellow, 12, nPosy);
                            nPosy += nRowLen;
                        }
                    }
                    strPaint = "";

                    //画离线删除时间
                    strPaint = "离线删除时间：";
                    if (maxexisttime != 0)
                    {
                        strPaint += maxexisttime.ToString();
                    }
                    else
                    {
                        strPaint = "";
                    }
                    if (strPaint != "")
                    {
                        g.DrawString(strPaint, fontTable[1], Brushes.White, 12, nPosy);
                        nPosy += nRowLen;
                    }
                    strPaint = "";

                    //画价格
                    if (canTrade)
                    {
                        Stream s = Assembly.GetExecutingAssembly().GetManifestResourceStream("sprViewer.bg.png");
                        Image imgbg = Image.FromStream(s);
                        g.DrawImage(imgbg, nWidth - 85, nPosy);
                        string sm1 = string.Format("{0}", price / 10000);
                        string sm2 = string.Format("{0}", ((price - (price / 10000) * 10000) / 100));
                        string sm3 = string.Format("{0}", price - (price / 100) * 100);
                        float sm1w = g.MeasureString(sm1, fontTable[1]).Width;
                        float sm2w = g.MeasureString(sm2, fontTable[1]).Width;
                        float sm3w = g.MeasureString(sm3, fontTable[1]).Width;
                        g.DrawString(sm1, fontTable[1], Brushes.White, nWidth - 82 - sm1w, nPosy + 2);
                        g.DrawString(sm2, fontTable[1], Brushes.White, nWidth - 50 - sm2w, nPosy + 2);
                        g.DrawString(sm3, fontTable[1], Brushes.White, nWidth - 18 - sm3w, nPosy + 2);
                    }
                }
            }

            ft.pb1.Image = img;
            ft.fitContoner();
        }

        List<string> skillDescs = new List<string>();
        public void AddOneDescItem(string strItem)
        {
            int n = skillDescs.Count;
            skillDescs.Insert(n, strItem);
        }
        List<string> skillShortDescs = new List<string>();
        public void AddOneShortDescItem(string strItem)
        {
            int n = skillShortDescs.Count;
            skillShortDescs.Insert(n, strItem);
        }
        public void ClearList()
        {
            skillShortDescs.Clear();
            skillDescs.Clear();
        }
        public void SetSkillInfo(
            int nSkillID, SqlConnection conn, string RootDir,            
            string skillName,string skillSpecialDesc,
            string skilllevel, int count, string strMaxLevel
            )
        {
            ft.SetSkillInfo(nSkillID, conn, RootDir, skillName, skillDescs, skillSpecialDesc, skillShortDescs, skilllevel, count, strMaxLevel);
        }
    }



    public class SPRItem 
    {
        public string tgaFileName = ""; // 输出图片文件（含路径，不含扩展名）
        public string tgaFolderName;
        public string sprFileName = ""; // 输入图片文件（含路径，含扩展名）
        public Hashtable TGAList = new Hashtable(); // 待预览的文件列表

        private DateTime time;
        private string cachePath = ""; // 缓存路径
        private int ipos = 1;
        private int utex_flag = 0;

        public bool m_forceRefresh = false;
        private static Hashtable saveImageTable = new Hashtable(); // 已经保存图片文件的数据表
        private static string iconFolder; // 图标文件夹
        private Image iconImage;

        /// <summary>
        /// 图标文件夹
        /// </summary>
        public static string IconFolder
        {
            set
            {
                iconFolder = value;
            }
        }

        // for UIText
        public SPRItem(string utexFileName, int nframe, DateTime time)
        {
            utex_flag = 1;
            this.time = time;
            this.sprFileName = utexFileName;
            this.tgaFolderName = string.Format(@"{0}\_cache\utex\", Application.StartupPath);
            this.tgaFileName = string.Format("{0}{1}{2}", tgaFolderName, ConverFileName(utexFileName), nframe.ToString());                        
            loadUtex(utexFileName, nframe);
        }

        public SPRItem(string utexFileName, int nframe)
        {
            utex_flag = 1;
            this.time = File.GetLastWriteTime(utexFileName);
            this.sprFileName = utexFileName;
            this.tgaFolderName = string.Format(@"{0}\_cache\utex\", Application.StartupPath);
            this.tgaFileName = string.Format("{0}{1}{2}", tgaFolderName, ConverFileName(utexFileName), nframe.ToString());  
            loadUtex(utexFileName, nframe);
        }

        // for SPR 
        public SPRItem(string sprFileName, DateTime time)
        {
            this.time = time;
            this.sprFileName = sprFileName;
            this.tgaFolderName = string.Format(@"{0}\_cache\spr\", Application.StartupPath);
            this.tgaFileName = string.Format("{0}{1}", tgaFolderName, ConverFileName(sprFileName));            
            loadTGA(sprFileName);
        }

        public SPRItem(string sprFileName)
        {
            this.time = File.GetLastWriteTime(sprFileName);
            this.sprFileName = sprFileName;
            this.tgaFolderName = string.Format(@"{0}\_cache\spr\", Application.StartupPath);
            this.tgaFileName = string.Format("{0}{1}", tgaFolderName, ConverFileName(sprFileName));
            loadTGA(sprFileName);
        }

        // 需要预览的图片类型
        public enum PIC_TYPE
        {
            MDL = 0,
            MTL = 1,
            MESH = 2,
            ANI = 3,
            NULL = 4
        }

        public Image IconImage
        {
            get
            {
                return iconImage;
            }
        }

        private bool Init(string strFileName, PIC_TYPE pictype, Test3DEngine engine, bool bForceRefresh)
        {            
            m_forceRefresh = bForceRefresh;

            this.time = File.GetLastWriteTime(strFileName);
            this.sprFileName = strFileName;
            string strExt = Enum.GetName(typeof(PIC_TYPE), pictype).ToLower();
            this.cachePath = string.Format(@"{0}\_cache\{1}\", Application.StartupPath, strExt);
            this.tgaFolderName = this.cachePath;
            this.tgaFileName = string.Format("{0}{1}", this.cachePath, ConverFileName(strFileName));

            if (!Directory.Exists(cachePath))
            {
                Directory.CreateDirectory(cachePath);
            }

            int nFrameCount = 0; //GetFrameCount(strFileName) 思豪的获取帧总数的函数
            strFileName = strFileName.ToLower();

            if (strFileName.EndsWith(".mdl") || strFileName.EndsWith(".mesh"))
            {
                nFrameCount = 1;
            }
            else if (strFileName.EndsWith(".ani") || strFileName.EndsWith(".tani"))
            {
                nFrameCount = 20;
            }

            for (int i = 1; i <= nFrameCount; i++)
            {
                string ii = i.ToString();

                while (ii.Length < 3)
                {
                    ii = "0" + ii;
                }
                
                TGAList[i.ToString()] = string.Format("{0}{1}.JPG", this.tgaFileName, ii);
            }

            string imageFileName = string.Format("{0}001.jpg", tgaFileName);

            switch (FileChanged())
            {
                case -1:
                    {
                        Image image = Helper.GetHelper().GetImageFromCache(GetTGAFileName(), "TGA");

                        if (image != null)
                        {
                            if (!File.Exists(imageFileName))
                            {
                                File.Create(imageFileName).Close();

                                if (saveImageTable[imageFileName] == null)
                                {
                                    try
                                    {
                                        image.Save(imageFileName, System.Drawing.Imaging.ImageFormat.Jpeg);
                                    }
                                    catch (ExternalException ex)
                                    {
                                        Helper.GetHelper().RecordLog(string.Format("保存图片文件{0}错误：{1}", imageFileName, ex.Message));
                                    }
                                    finally
                                    {
                                        saveImageTable[imageFileName] = "1";
                                    }                                                                        
                                }
                            }
                        }

                        iconImage = image;

                        break;
                    }
                case 0:
                    {
                        if (!File.Exists(imageFileName)) // 本地图片文件不存在的话需要创建出来
                        {
                            Image image = Helper.GetHelper().GetImageFromCache(GetTGAFileName(), "TGA");

                            if (image != null)
                            {
                                File.Create(imageFileName).Close();

                                if (saveImageTable[imageFileName] == null)
                                {
                                    try
                                    {
                                        image.Save(imageFileName, System.Drawing.Imaging.ImageFormat.Jpeg);
                                    }
                                    catch (ExternalException ex)
                                    {
                                        ;
                                    }
                                    finally
                                    {
                                        saveImageTable[imageFileName] = "1";
                                    }                                                                        
                                }
                            }

                            iconImage = image;
                        }

                        break;
                    }
                case 1:
                    {
                        try
                        {
                            engine.FileToImage(strFileName, this.tgaFileName);
                        }
                        catch(Exception ex)
                        {
                            Helper.GetHelper().RecordLog(string.Format("保存图片文件{0}错误：{1}", imageFileName, ex.Message));
                        }
                        finally
                        {
                            saveImageTable[imageFileName] = "1";
                        }

                        Helper.GetHelper().UpdateIcon(GetTGAFileName(), "TGA", imageFileName, GetLastWriteTime());  

                        break;
                    }
            }

            return true;
        }

        //for other
        public SPRItem(string strFileName, PIC_TYPE pictype, Test3DEngine engine)
        {
            Init(strFileName, pictype, engine, false);
        }
        public SPRItem(string strFileName, PIC_TYPE pictype, Test3DEngine engine, bool bForceRefresh)
        {
            Init(strFileName, pictype, engine, bForceRefresh);
        }


        public Image getNextImage()
        {
            if (TGAList.Count + 1 == ipos)
            {
                ipos = 1;
            }

            string spos = ipos.ToString();
            while (spos.Length < 3)
            {
                spos = "0" + spos;
            }

            Image img = null;

            try
            {
                string _strFile;
                if (utex_flag == 1)
                    _strFile = TGAList["1"].ToString();
                else
                    _strFile = string.Format("{0}{1}.JPG", this.tgaFileName, spos);

                if (File.Exists(_strFile))
                    img = Image.FromFile(_strFile);
            }
            finally
            {
                if (img == null)
                {
                    img = new Bitmap(10, 10);
                }                
            }

            ipos++;

            return img ;
        }


        private void loadUtex(string FileName, int nFrame)
        {
            cachePath = string.Format(@"{0}\_cache\utex\", Application.StartupPath);
            tgaFolderName = cachePath;

            if (!Directory.Exists(cachePath))
            {
                Directory.CreateDirectory(cachePath);
            }

            TGAList["1"] = string.Format("{0}.JPG", this.tgaFileName);
            FileInfo fi = new FileInfo(FileName);

            string imageFileName = string.Format("{0}.JPG", tgaFileName);

            switch (FileChanged())
            {
                case -1:
                    {
                        Image image = Helper.GetHelper().GetImageFromCache(GetTGAFileName(), "TGA");

                        if (image != null)
                        {
                            if (!File.Exists(imageFileName))
                            {
                                File.Create(imageFileName).Close();

                                if (saveImageTable[imageFileName] == null)
                                {
                                    try
                                    {
                                        image.Save(imageFileName, System.Drawing.Imaging.ImageFormat.Jpeg);
                                    }
                                    catch (ExternalException ex)
                                    {
                                        Helper.GetHelper().RecordLog(string.Format("保存图片文件{0}错误：{1}", imageFileName, ex.Message));
                                    }
                                    finally
                                    {
                                        saveImageTable[imageFileName] = "1";
                                    }                                                                       
                                }
                            }
                        }

                        iconImage = image;

                        break;
                    }
                case 0:
                    {
                        if (!File.Exists(imageFileName)) // 本地图片文件不存在的话需要创建出来
                        {
                            Image image = Helper.GetHelper().GetImageFromCache(GetTGAFileName(), "TGA");

                            if (image != null)
                            {
                                File.Create(imageFileName).Close();

                                if (saveImageTable[imageFileName] == null)
                                {
                                    try
                                    {
                                        image.Save(imageFileName, System.Drawing.Imaging.ImageFormat.Jpeg);
                                    }
                                    catch (ExternalException ex)
                                    {
                                        Helper.GetHelper().RecordLog(string.Format("保存图片文件{0}错误：{1}", imageFileName, ex.Message));
                                    }
                                    finally
                                    {
                                        saveImageTable[imageFileName] = "1";
                                    }                                                                        
                                }
                            }

                            iconImage = image;
                        }

                        break;
                    }
                case 1:
                    {
                        Image img = null;

                        try
                        {
                            img = tga.uitex2img(fi.DirectoryName, FileName, nFrame);

                            if (img != null)
                            {
                                img.Save(imageFileName, System.Drawing.Imaging.ImageFormat.Jpeg);
                            }                            
                        }
                        catch(Exception ex)
                        {
                            Helper.GetHelper().RecordLog(string.Format("保存图片文件{0}错误：{1}", imageFileName, ex.Message));
                        }
                        finally
                        {                            
                            saveImageTable[imageFileName] = "1";
                        }

                        iconImage = img;

                        if (img != null)
                        {
                            Helper.GetHelper().UpdateIcon(GetTGAFileName(), "TGA", imageFileName, GetLastWriteTime());
                        }                        

                        break;
                    }
            }
        }

        private void loadTGA(string FileName)
        {            
            cachePath = string.Format(@"{0}\_cache\spr\", Application.StartupPath);
            tgaFolderName = cachePath;

            if (!Directory.Exists(cachePath))
            {
                Directory.CreateDirectory(cachePath);
            }

            int nFrameCount = spr2tga.GetSPRFrameCount(FileName);
            for (int i = 1; i <= nFrameCount; i++)
            {
                string ii = i.ToString();
                while (ii.Length < 3)
                {
                    ii = "0" + ii;
                }
                TGAList[i.ToString()] = string.Format("{0}{1}.JPG", this.tgaFileName, ii);
            }

            string imageFileName = string.Format("{0}.JPG", tgaFileName);

            switch (FileChanged())
            {
                case -1:
                    {
                        Image image = Helper.GetHelper().GetImageFromCache(GetTGAFileName(), "TGA");

                        if (image != null)
                        {
                            if (!File.Exists(imageFileName))
                            {
                                File.Create(imageFileName).Close();

                                if (saveImageTable[imageFileName] == null)
                                {
                                    try
                                    {
                                        image.Save(imageFileName, System.Drawing.Imaging.ImageFormat.Jpeg);
                                    }
                                    catch (ExternalException ex)
                                    {
                                        Helper.GetHelper().RecordLog(string.Format("保存图片文件{0}错误：{1}", imageFileName, ex.Message));
                                    }
                                    finally
                                    {
                                        saveImageTable[imageFileName] = "1";
                                    }                                                                        
                                }
                            }
                        }

                        iconImage = image;

                        break;
                    }
                case 0:
                    {
                        if (!File.Exists(imageFileName)) // 本地图片文件不存在的话需要创建出来
                        {
                            Image image = Helper.GetHelper().GetImageFromCache(GetTGAFileName(), "TGA");

                            if (image != null)
                            {
                                File.Create(imageFileName).Close();

                                if (saveImageTable[imageFileName] == null)
                                {
                                    try
                                    {
                                        image.Save(imageFileName, System.Drawing.Imaging.ImageFormat.Jpeg);
                                    }
                                    catch (ExternalException ex)
                                    {
                                        Helper.GetHelper().RecordLog(string.Format("保存图片文件{0}错误：{1}", imageFileName, ex.Message));
                                    }
                                    finally
                                    {
                                        saveImageTable[imageFileName] = "1";
                                    }                                                                        
                                }
                            }
                            
                            iconImage = image;
                        }                        

                        break;
                    }
                case 1:
                    {
                        try
                        {
                            spr2tga.SaveSPRToFile(FileName, -1, true, 0, this.tgaFileName, "JPG");
                        }
                        catch (Exception ex)
                        {
                            Helper.GetHelper().RecordLog(string.Format("保存图片文件{0}错误：{1}", imageFileName, ex.Message));
                        }
                        finally
                        {
                            saveImageTable[imageFileName] = "1";
                        }

                        Helper.GetHelper().UpdateIcon(GetTGAFileName(), "TGA", imageFileName, GetLastWriteTime());

                        break;
                    }
            }
        }

        /// <summary>
        /// 获取文件的哈希值
        /// </summary>
        /// <returns>文件的哈希值</returns>
        private string ConverFileName(string fileName)
        {
            string newFileName = fileName;

            if (!string.IsNullOrEmpty(iconFolder)) // 只取相对路径
            {
                newFileName = fileName.Replace(iconFolder, "").TrimStart(new char[] { '\\' });
            }
                        
            newFileName = newFileName.Replace("\\", "_");
            newFileName = newFileName.Replace(":", "_"); // 预防在外部调用没有给文件夹赋值的问题

            return newFileName;
        }

        /// <summary>
        /// 获取TGA文件名
        /// </summary>
        /// <returns>TGA文件名</returns>
        private string GetTGAFileName()
        {
            string[] dataArray = tgaFileName.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);
            string fileName = dataArray[dataArray.Length - 1];

            return fileName;
        }

        /// <summary>
        /// 获取图片文件上次写入的时间
        /// </summary>
        /// <returns>上次写入的时间</returns>
        private DateTime GetLastWriteTime()
        {
            DateTime lastWriteTime = time;

            if (sprFileName.Contains(".UITex")) // UITex文件需要特殊处理，因为它是引用TGA文件
            {
                string referenceFileName = tga.GetTGAFilePath(sprFileName);

                if (referenceFileName != null)
                {
                    string newFileName = "";

                    string[] dataArray = sprFileName.Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);
                    
                    for (int i = 0; i < dataArray.Length; i++)
                    {
                        if (i != dataArray.Length - 1)
                        {
                            newFileName += string.Format("{0}\\", dataArray[i]);
                        }
                        else
                        {
                            newFileName += referenceFileName;
                        }
                    }

                    DateTime referenceFileTime = File.GetLastWriteTime(newFileName);

                    if (referenceFileTime > time)
                    {
                        lastWriteTime = referenceFileTime;
                    }
                }
            }

            return lastWriteTime;
        }

        /// <summary>
        /// 检查TGA文件是否改变过
        /// 返回值-1：当前图片写入时间比数据库里保存的图片的时间要旧，从数据库中取最新文件
        /// 返回值0：当前图片写入时间和数据库里保存的图片时间相等，不用进行任何操作
        /// 返回值1：当前图片写入时间比数据库里保存的图片时间要新，更新到数据库侯中
        /// </summary>
        /// <returns>比较值</returns>
        private int FileChanged()
        {
            // 强制刷新
            if (m_forceRefresh)
            {
                return 1;
            }            
           
            return Helper.GetHelper().RequireUpdateCache(GetTGAFileName(), "TGA", GetLastWriteTime());
        }
    };

    public static class spr2tga
    {
        [DllImport("SPR2TGA.dll", EntryPoint = "GetSPRFrameCount")]
        public static extern int GetSPRFrameCount(string strFilename);

        [DllImport("SPR2TGA.dll", EntryPoint = "SaveSPRToTGA")]
        public static extern bool SaveSPRToTGA(string strSprFileName, string strDescFileName, string FileExt);

        [DllImport("SPR2TGA.dll", EntryPoint = "SaveSPRToFile")]
        public static extern bool SaveSPRToFile(string src_path, int frame_index, bool premodulate, int color, string dst_path, string dst_ext);
    };

    public static class tga
    {
        private static Point rectangleLocation = new Point(0, 0); // 矩形位置，避免创建多次
        private static Size rectangleSize = new Size(0, 0); // 矩形大小，避免创建多次

        /// <summary>
        /// 获取TGA文件路径
        /// </summary>
        /// <param name="uitexFileName">UITex文件路径</param>
        /// <returns>TGA文件路径</returns>
        public static string GetTGAFilePath(string uitexFileName)
        {
            FileStream fileStream = File.Open(uitexFileName, FileMode.Open);
            BinaryReader binaryReader = new BinaryReader(fileStream);

            // 读文件名
            binaryReader.BaseStream.Seek(0x18, System.IO.SeekOrigin.Begin);
            byte[] buffer = binaryReader.ReadBytes(64);
            string tgaFilePath = System.Text.Encoding.Default.GetString(buffer);
            tgaFilePath = tgaFilePath.Substring(0, tgaFilePath.IndexOf('\0'));

            fileStream.Close();

            return tgaFilePath;
        }

        /// <summary>
        /// UITex转Img
        /// </summary>
        /// <param name="strIconDir">UITex的TGA所在目录，一般和UITex目录相同</param>
        /// <param name="filename">完整UITex文件名（含路径）</param>
        /// <param name="frame">第几帧</param>
        /// <returns></returns>
        public static Image uitex2img(string strIconDir, string filename, int frame)
        {
            //读取UITex文件，分析出当前帧坐标和长宽
            BinaryReader b = new BinaryReader(File.Open(filename, FileMode.Open));
            /*读文件名*/
            byte[] tb = new byte[64];
            b.BaseStream.Seek(0x18, System.IO.SeekOrigin.Begin);

            tb = b.ReadBytes(64);

            filename = System.Text.Encoding.Default.GetString(tb);            
            filename = string.Format(@"{0}\{1}", strIconDir, filename.Substring(0, filename.IndexOf('\0')));

            b.BaseStream.Seek(0x0c, System.IO.SeekOrigin.Begin);
            /*帧总数*/
            int allframe = b.ReadInt32();
            if (frame > allframe - 1)
            {
                b.Close();
                return null;
            }

            /*读帧坐标、高宽*/
            b.BaseStream.Seek(0x58 + frame * 0x14, SeekOrigin.Begin);
            int dwInfo = b.ReadInt32();
            int nLeft = b.ReadInt32();
            int nTop = b.ReadInt32();
            int nWidth = b.ReadInt32();
            int nHeight = b.ReadInt32();
            b.Close();

            // 裁剪图片
            rectangleLocation.X = nLeft;
            rectangleLocation.Y = nTop;
            rectangleSize.Width = nWidth;
            rectangleSize.Height = nHeight;
            Rectangle rectangle = new Rectangle(rectangleLocation, rectangleSize);
            Bitmap bmp = tga2img(filename) as Bitmap;
            Image cloneImage = null;
            Image image = null;

            if (bmp.Width != nWidth || bmp.Height != nHeight)
            {
                bool cloneSuccess = true;

                try
                {
                    cloneImage = bmp.Clone(rectangle, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
                }
                catch (OutOfMemoryException ex)
                {
                    cloneSuccess = false;
                    cloneImage = bmp; // 出错就用原图吧
                }

                if (cloneSuccess)
                {
                    bmp.Dispose();
                }                
            }
            else
            {
                cloneImage = bmp;
            }
                          
            if (cloneImage != null)
            {
                /*
                // 记录编码信息保存文件
                ImageCodecInfo myImageCodecInfo = GetEncoderInfo("image/jpeg");
                Encoder myEncoder = Encoder.Quality;
                EncoderParameters myEncoderParameters = new EncoderParameters(1);
                EncoderParameter myEncoderParameter = new EncoderParameter(myEncoder, 100L);
                myEncoderParameters.Param[0] = myEncoderParameter;

                MemoryStream ms = new MemoryStream();
                cloneImage.Save(ms, myImageCodecInfo, myEncoderParameters);
                cloneImage.Dispose();
                image = Image.FromStream(ms);
                */

                image = cloneImage;
            }

            return image;
        }

        private static ImageCodecInfo GetEncoderInfo(String mimeType)
        {
            int j;
            ImageCodecInfo[] encoders;
            encoders = ImageCodecInfo.GetImageEncoders();

            for (j = 0; j < encoders.Length; ++j)
            {
                if (encoders[j].MimeType == mimeType)
                {
                    return encoders[j];
                }
            }

            return null;
        }

        /// <summary>
        /// TGA转Image
        /// </summary>
        /// <param name="tgaFilename">TGA完整文件名</param>
        /// <returns>Image</returns>
        public static Image tga2img(string tgaFilename)
        {
            Paloma.TargaImage tgaImage = new Paloma.TargaImage(tgaFilename);
            return tgaImage.Image;

            //// 读取TGA文件，分析出长宽
            //BinaryReader b = new BinaryReader(File.Open(tgaFilename, FileMode.Open));
            //b.BaseStream.Seek(0x0C, System.IO.SeekOrigin.Begin);
            //int nWidth = b.ReadInt16();
            //int nHeight = b.ReadInt16();
            //b.Close();

            //// 送tgaPreview输出jpg文件
            //string tempFilePath = Path.GetTempFileName();

            //Panel panel = new Panel();
            //tgaPreview.Init(panel.Handle);
            //tgaPreview.SetFileNames(panel.Handle, tgaFilename + "\r\n", (UInt16)nWidth, (UInt16)nHeight);
            //tgaPreview.Render(panel.Handle);
            //tgaPreview.FrameMove(panel.Handle);
            //tgaPreview.ExportToFile(panel.Handle, (UInt16)nWidth, (UInt16)nHeight, tempFilePath);
            //tgaPreview.UnInit(panel.Handle);

            //// 载入jpg文件  
            //FileStream fs = new FileStream(tempFilePath, FileMode.Open, FileAccess.Read);
            //int byteLength = (int)fs.Length;
            //byte[] wf = new byte[byteLength];
            //fs.Read(wf, 0, byteLength);
            //fs.Close();
            //Image img = Image.FromStream(new MemoryStream(wf));
            //File.Delete(tempFilePath);

            //return img;
        }
    }  
    
    /// <summary>
    /// tgaPreview.dll 引用
    /// </summary>
    public static class tgaPreview
    {
        [DllImport("TGAPreview.dll")]
        public static extern void Init(IntPtr hRenderWnd);

        [DllImport("TGAPreview.dll")]
        public static extern void UnInit(IntPtr hRenderWnd);

        [DllImport("TGAPreview.dll")]
        public static extern void SetFileNames(IntPtr hRenderWnd, string Filenames, UInt16 nWidth, UInt16 nHeight);

        [DllImport("TGAPreview.dll")]
        public static extern void OnResize(IntPtr hRenderWnd);

        [DllImport("TGAPreview.dll")]
        public static extern void FrameMove(IntPtr hRenderWnd);  //可调整，每秒10帧，就每秒调用10次

        [DllImport("TGAPreview.dll")]
        public static extern void Render(IntPtr hRenderWnd);     //每秒30次调用

        [DllImport("TGAPreview.dll")]
        public static extern bool ExportToFile(IntPtr hRenderWnd, UInt16 nWidth, UInt16 nHeight, string Filenames);
    };
}
