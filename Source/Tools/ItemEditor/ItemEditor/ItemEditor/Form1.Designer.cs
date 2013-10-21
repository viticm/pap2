enum FilePath
{
    FILE_PATH_BEGIN = 0,
    FILE_INI = FILE_PATH_BEGIN,
    MAIN_INI,

    INI_FILE_BEGIN,
    CUSTOM_ITEM_INI_BEGIN = INI_FILE_BEGIN,
    CUSTOM_ARMOR_INI = CUSTOM_ITEM_INI_BEGIN,
    CUSTOM_TRINKET_INI,
    CUSTOM_WEAPON_INI,
    CUSTOM_ITEM_INI_END,

    CF_ITEM_INI_BEGIN = CUSTOM_ITEM_INI_END,
    CF_ARMOR_INI = CF_ITEM_INI_BEGIN,
    CF_TRINKET_INI,
    CF_WEAPON_INI,
    CF_ITEM_INI_END,

    GZ_INI_BEGIN = CF_ITEM_INI_END,
    GZ_MAGIC_INI = GZ_INI_BEGIN,
    GZ_WGDEFEND_INI,
    GZ_WGHURT_INI,
    GZ_VALUE_INI,
    GZ_ARMOR_INI,
    GZ_TRINKET_INI,
    GZ_WEAPON_INI,
    GZ_MAGICTYPE_INI,
    GZ_INI_END,

    TXT_FILE_BEGIN = GZ_INI_END,
    CUSTOM_ITEM_TXT_BEGIN = TXT_FILE_BEGIN,
    CUSTOM_ARMOR_TXT = CUSTOM_ITEM_TXT_BEGIN,
    CUSTOM_TRINKET_TXT,
    CUSTOM_WEAPON_TXT,
    CUSTOM_ITEM_TXT_END,

    CH_ITEM_TXT_BEGIN = CUSTOM_ITEM_TXT_END,
    CH_ARMOR_TXT = CH_ITEM_TXT_BEGIN,
    CH_TRINKET_TXT,
    CH_WEAPON_TXT,
    CH_ITEM_TXT_END,

    GZ_TXT_BEGIN = CH_ITEM_TXT_END,
    GZ_VALUE_TXT = GZ_TXT_BEGIN,
    GZ_MAGIC_TYPE_NAME_TXT,
    GZ_MAGIC_TYPE_VALUE_TXT,
    GZ_WGHURT_TXT,
    GZ_WGDEFEND_TXT,
    GZ_ARMOR_TXT,
    GZ_TRINKET_TXT,
    GZ_WEAPON_TXT,
    GZ_TXT_END,

    FILE_PATH_END = GZ_TXT_END,
};

enum MainTypeName
{
    MAIN_TYPE_NAME_BEGIN,

    MAIN_ITEM_TYPE_BEGIN = MAIN_TYPE_NAME_BEGIN,
    MAIN_ITEM_TYPE_ARMOR = MAIN_ITEM_TYPE_BEGIN,
    MAIN_ITEM_TYPE_TRINKET,
    MAIN_ITEM_TYPE_WEAPON,
    MAIN_ITEM_TYPE_END,

    SUB_ITEM_TYPE_BEGIN = MAIN_ITEM_TYPE_END,
    SUB_ITEM_TYPE_ARMOR_BEGIN = SUB_ITEM_TYPE_BEGIN,
    SUB_ITEM_TYPE_ARMOR_CLOTH = SUB_ITEM_TYPE_ARMOR_BEGIN,
    SUB_ITEM_TYPE_ARMOR_HEAD,
    SUB_ITEM_TYPE_ARMOR_GIRDLE,
    SUB_ITEM_TYPE_ARMOR_PANTS,
    SUB_ITEM_TYPE_ARMOR_SHOES,
    SUB_ITEM_TYPE_ARMOR_ARMS,
    SUB_ITEM_TYPE_ARMOR_END,

    SUB_ITEM_TYPE_TRINKET_BEGIN = SUB_ITEM_TYPE_ARMOR_END,
    SUB_ITEM_TYPE_TRINKET_RING = SUB_ITEM_TYPE_TRINKET_BEGIN,
    SUB_ITEM_TYPE_TRINKET_NECKLACE,
    SUB_ITEM_TYPE_TRINKET_PENDANT,
    SUB_ITEM_TYPE_TRINKET_END,

    SUB_ITEM_TYPE_WEAPON_BEGIN = SUB_ITEM_TYPE_TRINKET_END,
    SUB_ITEM_TYPE_WEAPON_RUNG = SUB_ITEM_TYPE_WEAPON_BEGIN,
    SUB_ITEM_TYPE_WEAPON_GUN,
    SUB_ITEM_TYPE_WEAPON_LONGSWORD,
    SUB_ITEM_TYPE_WEAPON_SHORTSWORD,
    SUB_ITEM_TYPE_WEAPON_MAULEY,
    SUB_ITEM_TYPE_WEAPON_DOUBLEHANDLE,
    SUB_ITEM_TYPE_WEAPON_END,
    SUB_ITEM_TYPE_END = SUB_ITEM_TYPE_WEAPON_END,

    SCHOOL_BEGIN = SUB_ITEM_TYPE_END,
    SCHOOL_SHAOLIN = SCHOOL_BEGIN,
    SCHOOL_WUDU,
    SCHOOL_WANHUA,
    SCHOOL_CANGJIAN,
    SCHOOL_TIANCE,
    SCHOOL_CHUNYANG,
    SCHOOL_GAIBANG,
    SCHOOL_TANGMEN,
    SCHOOL_QIXIU,
    SCHOOL_MINGJIAO,
    SCHOOL_END,

    QUAKITY_BEGIN = SCHOOL_END,
    QUAKITY_WHITE = QUAKITY_BEGIN,
    QUAKITY_BLUE,
    QUAKITY_GOLD,
    QUAKITY_PLATINUM,
    QUAKITY_END,

    LEASH_BEGIN = QUAKITY_END,
    LEASH_NONE = LEASH_BEGIN,
    LEASH_NOTFIXED,
    LEASH_FIXEDDONE,
    LEASH_FIXEDCOLLECT,
    LEASH_END,

    DEMAND_BEGIN = LEASH_END,
    DEMAND_TIZHI = DEMAND_BEGIN,
    DEMAND_LILIANG,
    DEMAND_GENGU,
    DEMAND_SHENFA,
    DEMAND_LEVEL,
    DEMAND_SCHOOL,
    DEMAND_END,

    BASE_DEMAND1_BEGIN = DEMAND_END,
    BASE_DEMAND1_WG_DEFAND = BASE_DEMAND1_BEGIN,
    BASE_DEMAND1_WG_HURT,
    BASE_DEMAND1_END,

    BASE_DEMAND2_BEGIN = BASE_DEMAND1_END,
    BASE_DEMAND2_NG_DEFAND = BASE_DEMAND2_BEGIN,
    BASE_DEMAND2_NG_HURT,
    BASE_DEMAND2_END,

    MAIN_TYPE_NAME_END,
};

namespace ItemEditor
{
    partial class Main
    {
        public struct Custon_ItemInfo
        {
            public int nID, nUiID, nRepresentID, nSetID;
            public int nKindType, nMainType, nSubType, nBindType, nQuality;
            public int nPrice, nLevel, nMaxDurability, nCanTrade;
            public string strName, strScriptNamePath;
            public string[] strBaseNeedType;
            public int[] nBaseNeedValue;
            public int[] nRequireInfo;
            public int[] nMagicType;
        };
        
        public struct CH_ItemInfo
        {
            public int nID, nLevel, nTotal_Value;
            public int nKindType, nMainType, nSubType;
            public string strName, strSchool, strQuality;
            public string[] strMagicType;
        };

        public struct MagicAttribute
        {
            public int nMagicId;
            public int nMagicMax;
            public int nMagicMin;
            public int nMagicValue;
            public string strMagicString;
            public string strMagicType;
            // sign(Add/Del)
            public bool bSign;
        };

        public struct ItemEntry // BaseMagic + SpeicalMagic <= 8
        {
            // show list
            public int nLevel;
            public string strType, strSubType, strSchool, strQuality;
            // show text
            public int nMagicNowValue, nMagicTotalValue, nMagicBaseValue;
            public int nWGHurtMax, nWGHurtMin;
            public int nWGDefendMax, nWGDefendMin;
            // Magic
            public int nItemBaseMagicCount;
            public int nItemMagicCount;
            public bool[] bItemMagicSige;
            public MagicAttribute[] ItemMagicSon;
            // SpeicalMagic son
            public int nRemainMagicValue;
            public int nMagicSonCount;
            public MagicAttribute[] ItemSpeMagicSon;
        };

        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            this.IDMoreLab = new System.Windows.Forms.Label();
            this.IDMoreMinText = new System.Windows.Forms.TextBox();
            this.IDMoreMaxText = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.MainTypeLab1 = new System.Windows.Forms.Label();
            this.MainTypeComB1 = new System.Windows.Forms.ComboBox();
            this.SubTypeComB1 = new System.Windows.Forms.ComboBox();
            this.SubTypeLab1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.LevelMoreMaxText = new System.Windows.Forms.TextBox();
            this.LevelMoreMinText = new System.Windows.Forms.TextBox();
            this.LevelMoreLab = new System.Windows.Forms.Label();
            this.QualityComb1 = new System.Windows.Forms.ComboBox();
            this.QualityLab1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.PriceMoreMaxText = new System.Windows.Forms.TextBox();
            this.PriceMoreMinText = new System.Windows.Forms.TextBox();
            this.PriceMoreLab = new System.Windows.Forms.Label();
            this.LeashComB1 = new System.Windows.Forms.ComboBox();
            this.LeashLab1 = new System.Windows.Forms.Label();
            this.SetIDText1 = new System.Windows.Forms.TextBox();
            this.SetIDLab1 = new System.Windows.Forms.Label();
            this.FindItem = new System.Windows.Forms.Button();
            this.CreateItem = new System.Windows.Forms.Button();
            this.SubTypeComb2 = new System.Windows.Forms.ComboBox();
            this.SubTypeLab2 = new System.Windows.Forms.Label();
            this.MainTypeComB2 = new System.Windows.Forms.ComboBox();
            this.TypeLab2 = new System.Windows.Forms.Label();
            this.SchoolComb1 = new System.Windows.Forms.ComboBox();
            this.SchoolLab1 = new System.Windows.Forms.Label();
            this.QualityComb2 = new System.Windows.Forms.ComboBox();
            this.QualityLab2 = new System.Windows.Forms.Label();
            this.LevelOnlyLab1 = new System.Windows.Forms.Label();
            this.FindItemResult = new System.Windows.Forms.Label();
            this.AddMagicLab = new System.Windows.Forms.Label();
            this.AddMagicBut = new System.Windows.Forms.Button();
            this.DelMagicBut = new System.Windows.Forms.Button();
            this.DelMagicLab = new System.Windows.Forms.Label();
            this.ItemInfoPlane = new System.Windows.Forms.Panel();
            this.DemValueText8 = new System.Windows.Forms.TextBox();
            this.DemValueLab8 = new System.Windows.Forms.Label();
            this.DemValueText7 = new System.Windows.Forms.TextBox();
            this.DemValueLab7 = new System.Windows.Forms.Label();
            this.DelItem = new System.Windows.Forms.Button();
            this.AddItem = new System.Windows.Forms.Button();
            this.DemValueText6 = new System.Windows.Forms.TextBox();
            this.DemValueText5 = new System.Windows.Forms.TextBox();
            this.DemValueText4 = new System.Windows.Forms.TextBox();
            this.DemValueText3 = new System.Windows.Forms.TextBox();
            this.DemValueText2 = new System.Windows.Forms.TextBox();
            this.DemValueText1 = new System.Windows.Forms.TextBox();
            this.DemValueLab6 = new System.Windows.Forms.Label();
            this.DemValueLab5 = new System.Windows.Forms.Label();
            this.DemValueLab4 = new System.Windows.Forms.Label();
            this.DemValueLab3 = new System.Windows.Forms.Label();
            this.DemValueLab2 = new System.Windows.Forms.Label();
            this.DemValueLab1 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.AttributeMaxText2 = new System.Windows.Forms.TextBox();
            this.AttributeMinText2 = new System.Windows.Forms.TextBox();
            this.AttributeComb2 = new System.Windows.Forms.ComboBox();
            this.AttributeLab2 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.AttributeMaxText1 = new System.Windows.Forms.TextBox();
            this.AttributeMinText1 = new System.Windows.Forms.TextBox();
            this.AttributeComb1 = new System.Windows.Forms.ComboBox();
            this.AttributeLab1 = new System.Windows.Forms.Label();
            this.NameText = new System.Windows.Forms.TextBox();
            this.NameLab = new System.Windows.Forms.Label();
            this.SetIDText2 = new System.Windows.Forms.TextBox();
            this.SetIDLab2 = new System.Windows.Forms.Label();
            this.RepIDText = new System.Windows.Forms.TextBox();
            this.RepIDLab = new System.Windows.Forms.Label();
            this.SchoolComb2 = new System.Windows.Forms.ComboBox();
            this.SchoolLab2 = new System.Windows.Forms.Label();
            this.UiIDText = new System.Windows.Forms.TextBox();
            this.UiIDLab = new System.Windows.Forms.Label();
            this.LeashComb2 = new System.Windows.Forms.ComboBox();
            this.LeashLab2 = new System.Windows.Forms.Label();
            this.PriceOnlyText2 = new System.Windows.Forms.TextBox();
            this.PriceOnlyLab2 = new System.Windows.Forms.Label();
            this.QualityComb3 = new System.Windows.Forms.ComboBox();
            this.QualityLab3 = new System.Windows.Forms.Label();
            this.LevelOnlyText2 = new System.Windows.Forms.TextBox();
            this.LevelOnlyLab2 = new System.Windows.Forms.Label();
            this.SubTypeComb3 = new System.Windows.Forms.ComboBox();
            this.SubTypeLab3 = new System.Windows.Forms.Label();
            this.MainTypeComB3 = new System.Windows.Forms.ComboBox();
            this.TypeLab3 = new System.Windows.Forms.Label();
            this.IDOnlyText = new System.Windows.Forms.TextBox();
            this.IDOnlyLab = new System.Windows.Forms.Label();
            this.ItemInfoLab = new System.Windows.Forms.Label();
            this.AddMagicListb = new System.Windows.Forms.ListBox();
            this.CreatItemPlane = new System.Windows.Forms.Panel();
            this.FindItemPlane = new System.Windows.Forms.Panel();
            this.MagicPlane = new System.Windows.Forms.Panel();
            this.ItemValueText = new System.Windows.Forms.TextBox();
            this.RemainMagicValue = new System.Windows.Forms.Label();
            this.ItemTotalValueLab = new System.Windows.Forms.Label();
            this.ItemValueLab = new System.Windows.Forms.Label();
            this.DelMagicListB = new System.Windows.Forms.ListBox();
            this.FindItemResultPlane = new System.Windows.Forms.Panel();
            this.FindItemResultListB = new System.Windows.Forms.ListBox();
            this.OpenFile = new System.Windows.Forms.Button();
            this.About = new System.Windows.Forms.Button();
            this.CreateItemResultPanel = new System.Windows.Forms.Panel();
            this.CreateItemResultListB = new System.Windows.Forms.ListBox();
            this.CreatItemResultLab = new System.Windows.Forms.Label();
            this.LevelMinOnlyText1 = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.LevelMaxOnlyText1 = new System.Windows.Forms.TextBox();
            this.ItemInfoPlane.SuspendLayout();
            this.CreatItemPlane.SuspendLayout();
            this.FindItemPlane.SuspendLayout();
            this.MagicPlane.SuspendLayout();
            this.FindItemResultPlane.SuspendLayout();
            this.CreateItemResultPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // folderBrowserDialog1
            // 
            this.folderBrowserDialog1.RootFolder = System.Environment.SpecialFolder.MyComputer;
            // 
            // IDMoreLab
            // 
            this.IDMoreLab.Location = new System.Drawing.Point(11, 8);
            this.IDMoreLab.Name = "IDMoreLab";
            this.IDMoreLab.Size = new System.Drawing.Size(41, 18);
            this.IDMoreLab.TabIndex = 3;
            this.IDMoreLab.Text = "ID";
            // 
            // IDMoreMinText
            // 
            this.IDMoreMinText.Location = new System.Drawing.Point(55, 5);
            this.IDMoreMinText.Name = "IDMoreMinText";
            this.IDMoreMinText.Size = new System.Drawing.Size(37, 21);
            this.IDMoreMinText.TabIndex = 4;
            // 
            // IDMoreMaxText
            // 
            this.IDMoreMaxText.Location = new System.Drawing.Point(119, 5);
            this.IDMoreMaxText.Name = "IDMoreMaxText";
            this.IDMoreMaxText.Size = new System.Drawing.Size(37, 21);
            this.IDMoreMaxText.TabIndex = 5;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(93, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(20, 18);
            this.label1.TabIndex = 6;
            this.label1.Text = " - ";
            // 
            // MainTypeLab1
            // 
            this.MainTypeLab1.Location = new System.Drawing.Point(9, 39);
            this.MainTypeLab1.Name = "MainTypeLab1";
            this.MainTypeLab1.Size = new System.Drawing.Size(47, 20);
            this.MainTypeLab1.TabIndex = 7;
            this.MainTypeLab1.Text = "Type*";
            // 
            // MainTypeComB1
            // 
            this.MainTypeComB1.FormattingEnabled = true;
            this.MainTypeComB1.Location = new System.Drawing.Point(55, 36);
            this.MainTypeComB1.Name = "MainTypeComB1";
            this.MainTypeComB1.Size = new System.Drawing.Size(101, 20);
            this.MainTypeComB1.TabIndex = 8;
            this.MainTypeComB1.SelectedIndexChanged += new System.EventHandler(this.MainTypeComB1_SelectedIndexChanged);
            // 
            // SubTypeComB1
            // 
            this.SubTypeComB1.FormattingEnabled = true;
            this.SubTypeComB1.Location = new System.Drawing.Point(55, 62);
            this.SubTypeComB1.Name = "SubTypeComB1";
            this.SubTypeComB1.Size = new System.Drawing.Size(101, 20);
            this.SubTypeComB1.TabIndex = 10;
            // 
            // SubTypeLab1
            // 
            this.SubTypeLab1.Location = new System.Drawing.Point(9, 65);
            this.SubTypeLab1.Name = "SubTypeLab1";
            this.SubTypeLab1.Size = new System.Drawing.Size(47, 20);
            this.SubTypeLab1.TabIndex = 9;
            this.SubTypeLab1.Text = "SubType";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(93, 91);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(20, 18);
            this.label2.TabIndex = 14;
            this.label2.Text = " - ";
            // 
            // LevelMoreMaxText
            // 
            this.LevelMoreMaxText.Location = new System.Drawing.Point(119, 88);
            this.LevelMoreMaxText.Name = "LevelMoreMaxText";
            this.LevelMoreMaxText.Size = new System.Drawing.Size(37, 21);
            this.LevelMoreMaxText.TabIndex = 13;
            // 
            // LevelMoreMinText
            // 
            this.LevelMoreMinText.Location = new System.Drawing.Point(55, 88);
            this.LevelMoreMinText.Name = "LevelMoreMinText";
            this.LevelMoreMinText.Size = new System.Drawing.Size(37, 21);
            this.LevelMoreMinText.TabIndex = 12;
            // 
            // LevelMoreLab
            // 
            this.LevelMoreLab.Location = new System.Drawing.Point(11, 91);
            this.LevelMoreLab.Name = "LevelMoreLab";
            this.LevelMoreLab.Size = new System.Drawing.Size(41, 18);
            this.LevelMoreLab.TabIndex = 11;
            this.LevelMoreLab.Text = "Level";
            // 
            // QualityComb1
            // 
            this.QualityComb1.FormattingEnabled = true;
            this.QualityComb1.Location = new System.Drawing.Point(55, 115);
            this.QualityComb1.Name = "QualityComb1";
            this.QualityComb1.Size = new System.Drawing.Size(101, 20);
            this.QualityComb1.TabIndex = 16;
            // 
            // QualityLab1
            // 
            this.QualityLab1.Location = new System.Drawing.Point(9, 118);
            this.QualityLab1.Name = "QualityLab1";
            this.QualityLab1.Size = new System.Drawing.Size(47, 20);
            this.QualityLab1.TabIndex = 15;
            this.QualityLab1.Text = "Quality";
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(93, 144);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(20, 18);
            this.label3.TabIndex = 20;
            this.label3.Text = " - ";
            // 
            // PriceMoreMaxText
            // 
            this.PriceMoreMaxText.Location = new System.Drawing.Point(119, 141);
            this.PriceMoreMaxText.Name = "PriceMoreMaxText";
            this.PriceMoreMaxText.Size = new System.Drawing.Size(37, 21);
            this.PriceMoreMaxText.TabIndex = 19;
            // 
            // PriceMoreMinText
            // 
            this.PriceMoreMinText.Location = new System.Drawing.Point(55, 141);
            this.PriceMoreMinText.Name = "PriceMoreMinText";
            this.PriceMoreMinText.Size = new System.Drawing.Size(37, 21);
            this.PriceMoreMinText.TabIndex = 18;
            // 
            // PriceMoreLab
            // 
            this.PriceMoreLab.Location = new System.Drawing.Point(11, 144);
            this.PriceMoreLab.Name = "PriceMoreLab";
            this.PriceMoreLab.Size = new System.Drawing.Size(41, 18);
            this.PriceMoreLab.TabIndex = 17;
            this.PriceMoreLab.Text = "Price";
            // 
            // LeashComB1
            // 
            this.LeashComB1.FormattingEnabled = true;
            this.LeashComB1.Location = new System.Drawing.Point(55, 165);
            this.LeashComB1.Name = "LeashComB1";
            this.LeashComB1.Size = new System.Drawing.Size(101, 20);
            this.LeashComB1.TabIndex = 22;
            // 
            // LeashLab1
            // 
            this.LeashLab1.Location = new System.Drawing.Point(9, 168);
            this.LeashLab1.Name = "LeashLab1";
            this.LeashLab1.Size = new System.Drawing.Size(47, 20);
            this.LeashLab1.TabIndex = 21;
            this.LeashLab1.Text = "Leash";
            // 
            // SetIDText1
            // 
            this.SetIDText1.Location = new System.Drawing.Point(55, 191);
            this.SetIDText1.Name = "SetIDText1";
            this.SetIDText1.Size = new System.Drawing.Size(101, 21);
            this.SetIDText1.TabIndex = 24;
            // 
            // SetIDLab1
            // 
            this.SetIDLab1.Location = new System.Drawing.Point(11, 194);
            this.SetIDLab1.Name = "SetIDLab1";
            this.SetIDLab1.Size = new System.Drawing.Size(41, 18);
            this.SetIDLab1.TabIndex = 23;
            this.SetIDLab1.Text = "SetID";
            // 
            // FindItem
            // 
            this.FindItem.Location = new System.Drawing.Point(55, 225);
            this.FindItem.Name = "FindItem";
            this.FindItem.Size = new System.Drawing.Size(91, 23);
            this.FindItem.TabIndex = 25;
            this.FindItem.Text = "FindItem";
            this.FindItem.UseVisualStyleBackColor = true;
            this.FindItem.Click += new System.EventHandler(this.FindItem_Click);
            // 
            // CreateItem
            // 
            this.CreateItem.Location = new System.Drawing.Point(55, 151);
            this.CreateItem.Name = "CreateItem";
            this.CreateItem.Size = new System.Drawing.Size(91, 23);
            this.CreateItem.TabIndex = 26;
            this.CreateItem.Text = "CreateItem";
            this.CreateItem.UseVisualStyleBackColor = true;
            this.CreateItem.Click += new System.EventHandler(this.CreateItem_Click);
            // 
            // SubTypeComb2
            // 
            this.SubTypeComb2.FormattingEnabled = true;
            this.SubTypeComb2.Location = new System.Drawing.Point(55, 39);
            this.SubTypeComb2.Name = "SubTypeComb2";
            this.SubTypeComb2.Size = new System.Drawing.Size(101, 20);
            this.SubTypeComb2.TabIndex = 30;
            // 
            // SubTypeLab2
            // 
            this.SubTypeLab2.Location = new System.Drawing.Point(-2, 42);
            this.SubTypeLab2.Name = "SubTypeLab2";
            this.SubTypeLab2.Size = new System.Drawing.Size(56, 20);
            this.SubTypeLab2.TabIndex = 29;
            this.SubTypeLab2.Text = "SubType*";
            // 
            // MainTypeComB2
            // 
            this.MainTypeComB2.FormattingEnabled = true;
            this.MainTypeComB2.Location = new System.Drawing.Point(55, 13);
            this.MainTypeComB2.Name = "MainTypeComB2";
            this.MainTypeComB2.Size = new System.Drawing.Size(101, 20);
            this.MainTypeComB2.TabIndex = 28;
            this.MainTypeComB2.SelectedIndexChanged += new System.EventHandler(this.MainTypeComB2_SelectedIndexChanged);
            // 
            // TypeLab2
            // 
            this.TypeLab2.Location = new System.Drawing.Point(0, 16);
            this.TypeLab2.Name = "TypeLab2";
            this.TypeLab2.Size = new System.Drawing.Size(56, 20);
            this.TypeLab2.TabIndex = 27;
            this.TypeLab2.Text = "Type*";
            // 
            // SchoolComb1
            // 
            this.SchoolComb1.FormattingEnabled = true;
            this.SchoolComb1.Location = new System.Drawing.Point(55, 90);
            this.SchoolComb1.Name = "SchoolComb1";
            this.SchoolComb1.Size = new System.Drawing.Size(101, 20);
            this.SchoolComb1.TabIndex = 32;
            // 
            // SchoolLab1
            // 
            this.SchoolLab1.Location = new System.Drawing.Point(0, 95);
            this.SchoolLab1.Name = "SchoolLab1";
            this.SchoolLab1.Size = new System.Drawing.Size(56, 20);
            this.SchoolLab1.TabIndex = 31;
            this.SchoolLab1.Text = "School*";
            // 
            // QualityComb2
            // 
            this.QualityComb2.FormattingEnabled = true;
            this.QualityComb2.Location = new System.Drawing.Point(55, 65);
            this.QualityComb2.Name = "QualityComb2";
            this.QualityComb2.Size = new System.Drawing.Size(101, 20);
            this.QualityComb2.TabIndex = 34;
            // 
            // QualityLab2
            // 
            this.QualityLab2.Location = new System.Drawing.Point(0, 65);
            this.QualityLab2.Name = "QualityLab2";
            this.QualityLab2.Size = new System.Drawing.Size(56, 20);
            this.QualityLab2.TabIndex = 33;
            this.QualityLab2.Text = "Quality*";
            // 
            // LevelOnlyLab1
            // 
            this.LevelOnlyLab1.Location = new System.Drawing.Point(0, 120);
            this.LevelOnlyLab1.Name = "LevelOnlyLab1";
            this.LevelOnlyLab1.Size = new System.Drawing.Size(52, 18);
            this.LevelOnlyLab1.TabIndex = 35;
            this.LevelOnlyLab1.Text = "Level";
            // 
            // FindItemResult
            // 
            this.FindItemResult.Location = new System.Drawing.Point(3, 10);
            this.FindItemResult.Name = "FindItemResult";
            this.FindItemResult.Size = new System.Drawing.Size(88, 17);
            this.FindItemResult.TabIndex = 37;
            this.FindItemResult.Text = "FindItemResult";
            // 
            // AddMagicLab
            // 
            this.AddMagicLab.Location = new System.Drawing.Point(21, 11);
            this.AddMagicLab.Name = "AddMagicLab";
            this.AddMagicLab.Size = new System.Drawing.Size(84, 19);
            this.AddMagicLab.TabIndex = 38;
            this.AddMagicLab.Text = "AddMagic";
            // 
            // AddMagicBut
            // 
            this.AddMagicBut.Location = new System.Drawing.Point(23, 192);
            this.AddMagicBut.Name = "AddMagicBut";
            this.AddMagicBut.Size = new System.Drawing.Size(65, 24);
            this.AddMagicBut.TabIndex = 39;
            this.AddMagicBut.Text = "AddMagic";
            this.AddMagicBut.UseVisualStyleBackColor = true;
            this.AddMagicBut.Click += new System.EventHandler(this.AddMagicBut_Click);
            // 
            // DelMagicBut
            // 
            this.DelMagicBut.Location = new System.Drawing.Point(14, 405);
            this.DelMagicBut.Name = "DelMagicBut";
            this.DelMagicBut.Size = new System.Drawing.Size(65, 24);
            this.DelMagicBut.TabIndex = 40;
            this.DelMagicBut.Text = "DelMagic";
            this.DelMagicBut.UseVisualStyleBackColor = true;
            this.DelMagicBut.Click += new System.EventHandler(this.DelMagicBut_Click);
            // 
            // DelMagicLab
            // 
            this.DelMagicLab.Location = new System.Drawing.Point(21, 222);
            this.DelMagicLab.Name = "DelMagicLab";
            this.DelMagicLab.Size = new System.Drawing.Size(84, 17);
            this.DelMagicLab.TabIndex = 41;
            this.DelMagicLab.Text = "DelMagic";
            // 
            // ItemInfoPlane
            // 
            this.ItemInfoPlane.Controls.Add(this.DemValueText8);
            this.ItemInfoPlane.Controls.Add(this.DemValueLab8);
            this.ItemInfoPlane.Controls.Add(this.DemValueText7);
            this.ItemInfoPlane.Controls.Add(this.DemValueLab7);
            this.ItemInfoPlane.Controls.Add(this.DelItem);
            this.ItemInfoPlane.Controls.Add(this.AddItem);
            this.ItemInfoPlane.Controls.Add(this.DemValueText6);
            this.ItemInfoPlane.Controls.Add(this.DemValueText5);
            this.ItemInfoPlane.Controls.Add(this.DemValueText4);
            this.ItemInfoPlane.Controls.Add(this.DemValueText3);
            this.ItemInfoPlane.Controls.Add(this.DemValueText2);
            this.ItemInfoPlane.Controls.Add(this.DemValueText1);
            this.ItemInfoPlane.Controls.Add(this.DemValueLab6);
            this.ItemInfoPlane.Controls.Add(this.DemValueLab5);
            this.ItemInfoPlane.Controls.Add(this.DemValueLab4);
            this.ItemInfoPlane.Controls.Add(this.DemValueLab3);
            this.ItemInfoPlane.Controls.Add(this.DemValueLab2);
            this.ItemInfoPlane.Controls.Add(this.DemValueLab1);
            this.ItemInfoPlane.Controls.Add(this.label5);
            this.ItemInfoPlane.Controls.Add(this.AttributeMaxText2);
            this.ItemInfoPlane.Controls.Add(this.AttributeMinText2);
            this.ItemInfoPlane.Controls.Add(this.AttributeComb2);
            this.ItemInfoPlane.Controls.Add(this.AttributeLab2);
            this.ItemInfoPlane.Controls.Add(this.label4);
            this.ItemInfoPlane.Controls.Add(this.AttributeMaxText1);
            this.ItemInfoPlane.Controls.Add(this.AttributeMinText1);
            this.ItemInfoPlane.Controls.Add(this.AttributeComb1);
            this.ItemInfoPlane.Controls.Add(this.AttributeLab1);
            this.ItemInfoPlane.Controls.Add(this.NameText);
            this.ItemInfoPlane.Controls.Add(this.NameLab);
            this.ItemInfoPlane.Controls.Add(this.SetIDText2);
            this.ItemInfoPlane.Controls.Add(this.SetIDLab2);
            this.ItemInfoPlane.Controls.Add(this.RepIDText);
            this.ItemInfoPlane.Controls.Add(this.RepIDLab);
            this.ItemInfoPlane.Controls.Add(this.SchoolComb2);
            this.ItemInfoPlane.Controls.Add(this.SchoolLab2);
            this.ItemInfoPlane.Controls.Add(this.UiIDText);
            this.ItemInfoPlane.Controls.Add(this.UiIDLab);
            this.ItemInfoPlane.Controls.Add(this.LeashComb2);
            this.ItemInfoPlane.Controls.Add(this.LeashLab2);
            this.ItemInfoPlane.Controls.Add(this.PriceOnlyText2);
            this.ItemInfoPlane.Controls.Add(this.PriceOnlyLab2);
            this.ItemInfoPlane.Controls.Add(this.QualityComb3);
            this.ItemInfoPlane.Controls.Add(this.QualityLab3);
            this.ItemInfoPlane.Controls.Add(this.LevelOnlyText2);
            this.ItemInfoPlane.Controls.Add(this.LevelOnlyLab2);
            this.ItemInfoPlane.Controls.Add(this.SubTypeComb3);
            this.ItemInfoPlane.Controls.Add(this.SubTypeLab3);
            this.ItemInfoPlane.Controls.Add(this.MainTypeComB3);
            this.ItemInfoPlane.Controls.Add(this.TypeLab3);
            this.ItemInfoPlane.Controls.Add(this.IDOnlyText);
            this.ItemInfoPlane.Controls.Add(this.IDOnlyLab);
            this.ItemInfoPlane.Controls.Add(this.ItemInfoLab);
            this.ItemInfoPlane.Location = new System.Drawing.Point(702, 27);
            this.ItemInfoPlane.Name = "ItemInfoPlane";
            this.ItemInfoPlane.Size = new System.Drawing.Size(302, 431);
            this.ItemInfoPlane.TabIndex = 108;
            // 
            // DemValueText8
            // 
            this.DemValueText8.Location = new System.Drawing.Point(203, 242);
            this.DemValueText8.Name = "DemValueText8";
            this.DemValueText8.Size = new System.Drawing.Size(84, 21);
            this.DemValueText8.TabIndex = 178;
            // 
            // DemValueLab8
            // 
            this.DemValueLab8.Location = new System.Drawing.Point(159, 246);
            this.DemValueLab8.Name = "DemValueLab8";
            this.DemValueLab8.Size = new System.Drawing.Size(45, 17);
            this.DemValueLab8.TabIndex = 177;
            this.DemValueLab8.Text = "Value8";
            // 
            // DemValueText7
            // 
            this.DemValueText7.Location = new System.Drawing.Point(203, 214);
            this.DemValueText7.Name = "DemValueText7";
            this.DemValueText7.Size = new System.Drawing.Size(84, 21);
            this.DemValueText7.TabIndex = 176;
            // 
            // DemValueLab7
            // 
            this.DemValueLab7.Location = new System.Drawing.Point(159, 218);
            this.DemValueLab7.Name = "DemValueLab7";
            this.DemValueLab7.Size = new System.Drawing.Size(45, 17);
            this.DemValueLab7.TabIndex = 175;
            this.DemValueLab7.Text = "Value7";
            // 
            // DelItem
            // 
            this.DelItem.Location = new System.Drawing.Point(167, 387);
            this.DelItem.Name = "DelItem";
            this.DelItem.Size = new System.Drawing.Size(120, 23);
            this.DelItem.TabIndex = 174;
            this.DelItem.Text = "DelItem";
            this.DelItem.UseVisualStyleBackColor = true;
            this.DelItem.Click += new System.EventHandler(this.DelItem_Click);
            // 
            // AddItem
            // 
            this.AddItem.Location = new System.Drawing.Point(24, 386);
            this.AddItem.Name = "AddItem";
            this.AddItem.Size = new System.Drawing.Size(120, 25);
            this.AddItem.TabIndex = 173;
            this.AddItem.Text = "AddItem";
            this.AddItem.UseVisualStyleBackColor = true;
            this.AddItem.Click += new System.EventHandler(this.AddItem_Click);
            // 
            // DemValueText6
            // 
            this.DemValueText6.Location = new System.Drawing.Point(203, 187);
            this.DemValueText6.Name = "DemValueText6";
            this.DemValueText6.Size = new System.Drawing.Size(84, 21);
            this.DemValueText6.TabIndex = 172;
            // 
            // DemValueText5
            // 
            this.DemValueText5.Location = new System.Drawing.Point(203, 161);
            this.DemValueText5.Name = "DemValueText5";
            this.DemValueText5.Size = new System.Drawing.Size(84, 21);
            this.DemValueText5.TabIndex = 171;
            // 
            // DemValueText4
            // 
            this.DemValueText4.Location = new System.Drawing.Point(203, 134);
            this.DemValueText4.Name = "DemValueText4";
            this.DemValueText4.Size = new System.Drawing.Size(84, 21);
            this.DemValueText4.TabIndex = 170;
            // 
            // DemValueText3
            // 
            this.DemValueText3.Location = new System.Drawing.Point(203, 105);
            this.DemValueText3.Name = "DemValueText3";
            this.DemValueText3.Size = new System.Drawing.Size(84, 21);
            this.DemValueText3.TabIndex = 169;
            // 
            // DemValueText2
            // 
            this.DemValueText2.Location = new System.Drawing.Point(203, 78);
            this.DemValueText2.Name = "DemValueText2";
            this.DemValueText2.Size = new System.Drawing.Size(84, 21);
            this.DemValueText2.TabIndex = 168;
            // 
            // DemValueText1
            // 
            this.DemValueText1.Location = new System.Drawing.Point(203, 51);
            this.DemValueText1.Name = "DemValueText1";
            this.DemValueText1.Size = new System.Drawing.Size(84, 21);
            this.DemValueText1.TabIndex = 167;
            // 
            // DemValueLab6
            // 
            this.DemValueLab6.Location = new System.Drawing.Point(159, 191);
            this.DemValueLab6.Name = "DemValueLab6";
            this.DemValueLab6.Size = new System.Drawing.Size(45, 17);
            this.DemValueLab6.TabIndex = 165;
            this.DemValueLab6.Text = "Value6";
            // 
            // DemValueLab5
            // 
            this.DemValueLab5.Location = new System.Drawing.Point(159, 164);
            this.DemValueLab5.Name = "DemValueLab5";
            this.DemValueLab5.Size = new System.Drawing.Size(45, 17);
            this.DemValueLab5.TabIndex = 161;
            this.DemValueLab5.Text = "Value5";
            // 
            // DemValueLab4
            // 
            this.DemValueLab4.Location = new System.Drawing.Point(159, 137);
            this.DemValueLab4.Name = "DemValueLab4";
            this.DemValueLab4.Size = new System.Drawing.Size(45, 17);
            this.DemValueLab4.TabIndex = 157;
            this.DemValueLab4.Text = "Value4";
            // 
            // DemValueLab3
            // 
            this.DemValueLab3.Location = new System.Drawing.Point(159, 108);
            this.DemValueLab3.Name = "DemValueLab3";
            this.DemValueLab3.Size = new System.Drawing.Size(45, 17);
            this.DemValueLab3.TabIndex = 153;
            this.DemValueLab3.Text = "Value3";
            // 
            // DemValueLab2
            // 
            this.DemValueLab2.Location = new System.Drawing.Point(159, 81);
            this.DemValueLab2.Name = "DemValueLab2";
            this.DemValueLab2.Size = new System.Drawing.Size(45, 17);
            this.DemValueLab2.TabIndex = 149;
            this.DemValueLab2.Text = "Value2";
            // 
            // DemValueLab1
            // 
            this.DemValueLab1.Location = new System.Drawing.Point(159, 55);
            this.DemValueLab1.Name = "DemValueLab1";
            this.DemValueLab1.Size = new System.Drawing.Size(45, 17);
            this.DemValueLab1.TabIndex = 145;
            this.DemValueLab1.Text = "Value1";
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(241, 351);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(11, 18);
            this.label5.TabIndex = 142;
            this.label5.Text = "-";
            // 
            // AttributeMaxText2
            // 
            this.AttributeMaxText2.Location = new System.Drawing.Point(258, 348);
            this.AttributeMaxText2.Name = "AttributeMaxText2";
            this.AttributeMaxText2.Size = new System.Drawing.Size(29, 21);
            this.AttributeMaxText2.TabIndex = 141;
            // 
            // AttributeMinText2
            // 
            this.AttributeMinText2.Location = new System.Drawing.Point(203, 348);
            this.AttributeMinText2.Name = "AttributeMinText2";
            this.AttributeMinText2.Size = new System.Drawing.Size(32, 21);
            this.AttributeMinText2.TabIndex = 140;
            // 
            // AttributeComb2
            // 
            this.AttributeComb2.FormattingEnabled = true;
            this.AttributeComb2.Location = new System.Drawing.Point(203, 322);
            this.AttributeComb2.Name = "AttributeComb2";
            this.AttributeComb2.Size = new System.Drawing.Size(84, 20);
            this.AttributeComb2.TabIndex = 139;
            // 
            // AttributeLab2
            // 
            this.AttributeLab2.Location = new System.Drawing.Point(159, 325);
            this.AttributeLab2.Name = "AttributeLab2";
            this.AttributeLab2.Size = new System.Drawing.Size(45, 17);
            this.AttributeLab2.TabIndex = 138;
            this.AttributeLab2.Text = "Att2";
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(241, 298);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(11, 18);
            this.label4.TabIndex = 137;
            this.label4.Text = "-";
            // 
            // AttributeMaxText1
            // 
            this.AttributeMaxText1.Location = new System.Drawing.Point(258, 295);
            this.AttributeMaxText1.Name = "AttributeMaxText1";
            this.AttributeMaxText1.Size = new System.Drawing.Size(29, 21);
            this.AttributeMaxText1.TabIndex = 136;
            // 
            // AttributeMinText1
            // 
            this.AttributeMinText1.Location = new System.Drawing.Point(203, 295);
            this.AttributeMinText1.Name = "AttributeMinText1";
            this.AttributeMinText1.Size = new System.Drawing.Size(32, 21);
            this.AttributeMinText1.TabIndex = 135;
            // 
            // AttributeComb1
            // 
            this.AttributeComb1.FormattingEnabled = true;
            this.AttributeComb1.Location = new System.Drawing.Point(203, 269);
            this.AttributeComb1.Name = "AttributeComb1";
            this.AttributeComb1.Size = new System.Drawing.Size(84, 20);
            this.AttributeComb1.TabIndex = 134;
            // 
            // AttributeLab1
            // 
            this.AttributeLab1.Location = new System.Drawing.Point(159, 272);
            this.AttributeLab1.Name = "AttributeLab1";
            this.AttributeLab1.Size = new System.Drawing.Size(45, 17);
            this.AttributeLab1.TabIndex = 133;
            this.AttributeLab1.Text = "Att1";
            // 
            // NameText
            // 
            this.NameText.Location = new System.Drawing.Point(60, 343);
            this.NameText.Name = "NameText";
            this.NameText.Size = new System.Drawing.Size(84, 21);
            this.NameText.TabIndex = 132;
            // 
            // NameLab
            // 
            this.NameLab.Location = new System.Drawing.Point(16, 346);
            this.NameLab.Name = "NameLab";
            this.NameLab.Size = new System.Drawing.Size(41, 18);
            this.NameLab.TabIndex = 131;
            this.NameLab.Text = "Name";
            // 
            // SetIDText2
            // 
            this.SetIDText2.Location = new System.Drawing.Point(60, 316);
            this.SetIDText2.Name = "SetIDText2";
            this.SetIDText2.Size = new System.Drawing.Size(84, 21);
            this.SetIDText2.TabIndex = 130;
            // 
            // SetIDLab2
            // 
            this.SetIDLab2.Location = new System.Drawing.Point(16, 319);
            this.SetIDLab2.Name = "SetIDLab2";
            this.SetIDLab2.Size = new System.Drawing.Size(41, 18);
            this.SetIDLab2.TabIndex = 129;
            this.SetIDLab2.Text = "SetID";
            // 
            // RepIDText
            // 
            this.RepIDText.Location = new System.Drawing.Point(60, 289);
            this.RepIDText.Name = "RepIDText";
            this.RepIDText.Size = new System.Drawing.Size(84, 21);
            this.RepIDText.TabIndex = 128;
            // 
            // RepIDLab
            // 
            this.RepIDLab.Location = new System.Drawing.Point(16, 292);
            this.RepIDLab.Name = "RepIDLab";
            this.RepIDLab.Size = new System.Drawing.Size(41, 18);
            this.RepIDLab.TabIndex = 127;
            this.RepIDLab.Text = "RepID";
            // 
            // SchoolComb2
            // 
            this.SchoolComb2.FormattingEnabled = true;
            this.SchoolComb2.Location = new System.Drawing.Point(60, 134);
            this.SchoolComb2.Name = "SchoolComb2";
            this.SchoolComb2.Size = new System.Drawing.Size(84, 20);
            this.SchoolComb2.TabIndex = 126;
            // 
            // SchoolLab2
            // 
            this.SchoolLab2.Location = new System.Drawing.Point(14, 137);
            this.SchoolLab2.Name = "SchoolLab2";
            this.SchoolLab2.Size = new System.Drawing.Size(47, 20);
            this.SchoolLab2.TabIndex = 125;
            this.SchoolLab2.Text = "School";
            // 
            // UiIDText
            // 
            this.UiIDText.Location = new System.Drawing.Point(60, 262);
            this.UiIDText.Name = "UiIDText";
            this.UiIDText.Size = new System.Drawing.Size(84, 21);
            this.UiIDText.TabIndex = 124;
            // 
            // UiIDLab
            // 
            this.UiIDLab.Location = new System.Drawing.Point(16, 265);
            this.UiIDLab.Name = "UiIDLab";
            this.UiIDLab.Size = new System.Drawing.Size(41, 18);
            this.UiIDLab.TabIndex = 123;
            this.UiIDLab.Text = "UiID";
            // 
            // LeashComb2
            // 
            this.LeashComb2.FormattingEnabled = true;
            this.LeashComb2.Location = new System.Drawing.Point(60, 236);
            this.LeashComb2.Name = "LeashComb2";
            this.LeashComb2.Size = new System.Drawing.Size(84, 20);
            this.LeashComb2.TabIndex = 122;
            // 
            // LeashLab2
            // 
            this.LeashLab2.Location = new System.Drawing.Point(14, 239);
            this.LeashLab2.Name = "LeashLab2";
            this.LeashLab2.Size = new System.Drawing.Size(47, 20);
            this.LeashLab2.TabIndex = 121;
            this.LeashLab2.Text = "Leash";
            // 
            // PriceOnlyText2
            // 
            this.PriceOnlyText2.Location = new System.Drawing.Point(60, 212);
            this.PriceOnlyText2.Name = "PriceOnlyText2";
            this.PriceOnlyText2.Size = new System.Drawing.Size(84, 21);
            this.PriceOnlyText2.TabIndex = 120;
            // 
            // PriceOnlyLab2
            // 
            this.PriceOnlyLab2.Location = new System.Drawing.Point(16, 215);
            this.PriceOnlyLab2.Name = "PriceOnlyLab2";
            this.PriceOnlyLab2.Size = new System.Drawing.Size(41, 18);
            this.PriceOnlyLab2.TabIndex = 119;
            this.PriceOnlyLab2.Text = "Price";
            // 
            // QualityComb3
            // 
            this.QualityComb3.FormattingEnabled = true;
            this.QualityComb3.Location = new System.Drawing.Point(60, 159);
            this.QualityComb3.Name = "QualityComb3";
            this.QualityComb3.Size = new System.Drawing.Size(84, 20);
            this.QualityComb3.TabIndex = 118;
            // 
            // QualityLab3
            // 
            this.QualityLab3.Location = new System.Drawing.Point(14, 162);
            this.QualityLab3.Name = "QualityLab3";
            this.QualityLab3.Size = new System.Drawing.Size(47, 20);
            this.QualityLab3.TabIndex = 117;
            this.QualityLab3.Text = "Quality";
            // 
            // LevelOnlyText2
            // 
            this.LevelOnlyText2.Location = new System.Drawing.Point(60, 185);
            this.LevelOnlyText2.Name = "LevelOnlyText2";
            this.LevelOnlyText2.Size = new System.Drawing.Size(84, 21);
            this.LevelOnlyText2.TabIndex = 116;
            // 
            // LevelOnlyLab2
            // 
            this.LevelOnlyLab2.Location = new System.Drawing.Point(16, 188);
            this.LevelOnlyLab2.Name = "LevelOnlyLab2";
            this.LevelOnlyLab2.Size = new System.Drawing.Size(41, 18);
            this.LevelOnlyLab2.TabIndex = 115;
            this.LevelOnlyLab2.Text = "Level";
            // 
            // SubTypeComb3
            // 
            this.SubTypeComb3.FormattingEnabled = true;
            this.SubTypeComb3.Location = new System.Drawing.Point(60, 108);
            this.SubTypeComb3.Name = "SubTypeComb3";
            this.SubTypeComb3.Size = new System.Drawing.Size(84, 20);
            this.SubTypeComb3.TabIndex = 114;
            // 
            // SubTypeLab3
            // 
            this.SubTypeLab3.Location = new System.Drawing.Point(14, 111);
            this.SubTypeLab3.Name = "SubTypeLab3";
            this.SubTypeLab3.Size = new System.Drawing.Size(47, 20);
            this.SubTypeLab3.TabIndex = 113;
            this.SubTypeLab3.Text = "SubType";
            // 
            // MainTypeComB3
            // 
            this.MainTypeComB3.FormattingEnabled = true;
            this.MainTypeComB3.Location = new System.Drawing.Point(60, 82);
            this.MainTypeComB3.Name = "MainTypeComB3";
            this.MainTypeComB3.Size = new System.Drawing.Size(84, 20);
            this.MainTypeComB3.TabIndex = 112;
            this.MainTypeComB3.SelectedIndexChanged += new System.EventHandler(this.MainTypeComB3_SelectedIndexChanged);
            // 
            // TypeLab3
            // 
            this.TypeLab3.Location = new System.Drawing.Point(14, 85);
            this.TypeLab3.Name = "TypeLab3";
            this.TypeLab3.Size = new System.Drawing.Size(47, 20);
            this.TypeLab3.TabIndex = 111;
            this.TypeLab3.Text = "Type*";
            // 
            // IDOnlyText
            // 
            this.IDOnlyText.Location = new System.Drawing.Point(60, 51);
            this.IDOnlyText.Name = "IDOnlyText";
            this.IDOnlyText.Size = new System.Drawing.Size(84, 21);
            this.IDOnlyText.TabIndex = 110;
            // 
            // IDOnlyLab
            // 
            this.IDOnlyLab.Location = new System.Drawing.Point(16, 54);
            this.IDOnlyLab.Name = "IDOnlyLab";
            this.IDOnlyLab.Size = new System.Drawing.Size(41, 18);
            this.IDOnlyLab.TabIndex = 109;
            this.IDOnlyLab.Text = "ID*";
            // 
            // ItemInfoLab
            // 
            this.ItemInfoLab.Location = new System.Drawing.Point(14, 11);
            this.ItemInfoLab.Name = "ItemInfoLab";
            this.ItemInfoLab.Size = new System.Drawing.Size(84, 19);
            this.ItemInfoLab.TabIndex = 108;
            this.ItemInfoLab.Text = "ItemInfo";
            // 
            // AddMagicListb
            // 
            this.AddMagicListb.FormattingEnabled = true;
            this.AddMagicListb.HorizontalScrollbar = true;
            this.AddMagicListb.ItemHeight = 12;
            this.AddMagicListb.Location = new System.Drawing.Point(23, 28);
            this.AddMagicListb.Name = "AddMagicListb";
            this.AddMagicListb.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.AddMagicListb.Size = new System.Drawing.Size(478, 160);
            this.AddMagicListb.TabIndex = 109;
            // 
            // CreatItemPlane
            // 
            this.CreatItemPlane.Controls.Add(this.LevelMaxOnlyText1);
            this.CreatItemPlane.Controls.Add(this.label6);
            this.CreatItemPlane.Controls.Add(this.SubTypeComb2);
            this.CreatItemPlane.Controls.Add(this.CreateItem);
            this.CreatItemPlane.Controls.Add(this.TypeLab2);
            this.CreatItemPlane.Controls.Add(this.MainTypeComB2);
            this.CreatItemPlane.Controls.Add(this.SubTypeLab2);
            this.CreatItemPlane.Controls.Add(this.SchoolLab1);
            this.CreatItemPlane.Controls.Add(this.SchoolComb1);
            this.CreatItemPlane.Controls.Add(this.QualityLab2);
            this.CreatItemPlane.Controls.Add(this.LevelMinOnlyText1);
            this.CreatItemPlane.Controls.Add(this.QualityComb2);
            this.CreatItemPlane.Controls.Add(this.LevelOnlyLab1);
            this.CreatItemPlane.Location = new System.Drawing.Point(0, 281);
            this.CreatItemPlane.Name = "CreatItemPlane";
            this.CreatItemPlane.Size = new System.Drawing.Size(176, 180);
            this.CreatItemPlane.TabIndex = 110;
            // 
            // FindItemPlane
            // 
            this.FindItemPlane.Controls.Add(this.QualityComb1);
            this.FindItemPlane.Controls.Add(this.IDMoreLab);
            this.FindItemPlane.Controls.Add(this.IDMoreMinText);
            this.FindItemPlane.Controls.Add(this.IDMoreMaxText);
            this.FindItemPlane.Controls.Add(this.label1);
            this.FindItemPlane.Controls.Add(this.MainTypeLab1);
            this.FindItemPlane.Controls.Add(this.MainTypeComB1);
            this.FindItemPlane.Controls.Add(this.SubTypeLab1);
            this.FindItemPlane.Controls.Add(this.FindItem);
            this.FindItemPlane.Controls.Add(this.SubTypeComB1);
            this.FindItemPlane.Controls.Add(this.LevelMoreLab);
            this.FindItemPlane.Controls.Add(this.SetIDText1);
            this.FindItemPlane.Controls.Add(this.LevelMoreMinText);
            this.FindItemPlane.Controls.Add(this.SetIDLab1);
            this.FindItemPlane.Controls.Add(this.LevelMoreMaxText);
            this.FindItemPlane.Controls.Add(this.LeashComB1);
            this.FindItemPlane.Controls.Add(this.label2);
            this.FindItemPlane.Controls.Add(this.LeashLab1);
            this.FindItemPlane.Controls.Add(this.QualityLab1);
            this.FindItemPlane.Controls.Add(this.label3);
            this.FindItemPlane.Controls.Add(this.PriceMoreLab);
            this.FindItemPlane.Controls.Add(this.PriceMoreMaxText);
            this.FindItemPlane.Controls.Add(this.PriceMoreMinText);
            this.FindItemPlane.Location = new System.Drawing.Point(0, 27);
            this.FindItemPlane.Name = "FindItemPlane";
            this.FindItemPlane.Size = new System.Drawing.Size(176, 256);
            this.FindItemPlane.TabIndex = 111;
            // 
            // MagicPlane
            // 
            this.MagicPlane.Controls.Add(this.ItemValueText);
            this.MagicPlane.Controls.Add(this.RemainMagicValue);
            this.MagicPlane.Controls.Add(this.ItemTotalValueLab);
            this.MagicPlane.Controls.Add(this.ItemValueLab);
            this.MagicPlane.Controls.Add(this.DelMagicListB);
            this.MagicPlane.Controls.Add(this.AddMagicListb);
            this.MagicPlane.Controls.Add(this.AddMagicLab);
            this.MagicPlane.Controls.Add(this.AddMagicBut);
            this.MagicPlane.Controls.Add(this.DelMagicLab);
            this.MagicPlane.Controls.Add(this.DelMagicBut);
            this.MagicPlane.Location = new System.Drawing.Point(182, 27);
            this.MagicPlane.Name = "MagicPlane";
            this.MagicPlane.Size = new System.Drawing.Size(514, 434);
            this.MagicPlane.TabIndex = 112;
            // 
            // ItemValueText
            // 
            this.ItemValueText.Location = new System.Drawing.Point(132, 194);
            this.ItemValueText.Name = "ItemValueText";
            this.ItemValueText.Size = new System.Drawing.Size(79, 21);
            this.ItemValueText.TabIndex = 115;
            // 
            // RemainMagicValue
            // 
            this.RemainMagicValue.Location = new System.Drawing.Point(334, 198);
            this.RemainMagicValue.Name = "RemainMagicValue";
            this.RemainMagicValue.Size = new System.Drawing.Size(139, 23);
            this.RemainMagicValue.TabIndex = 114;
            this.RemainMagicValue.Text = "Remain : 0";
            // 
            // ItemTotalValueLab
            // 
            this.ItemTotalValueLab.Location = new System.Drawing.Point(217, 198);
            this.ItemTotalValueLab.Name = "ItemTotalValueLab";
            this.ItemTotalValueLab.Size = new System.Drawing.Size(111, 19);
            this.ItemTotalValueLab.TabIndex = 113;
            this.ItemTotalValueLab.Text = "Total : 0";
            // 
            // ItemValueLab
            // 
            this.ItemValueLab.Location = new System.Drawing.Point(93, 198);
            this.ItemValueLab.Name = "ItemValueLab";
            this.ItemValueLab.Size = new System.Drawing.Size(53, 19);
            this.ItemValueLab.TabIndex = 111;
            this.ItemValueLab.Text = "Value";
            // 
            // DelMagicListB
            // 
            this.DelMagicListB.FormattingEnabled = true;
            this.DelMagicListB.HorizontalScrollbar = true;
            this.DelMagicListB.ItemHeight = 12;
            this.DelMagicListB.Location = new System.Drawing.Point(23, 239);
            this.DelMagicListB.Name = "DelMagicListB";
            this.DelMagicListB.Size = new System.Drawing.Size(478, 160);
            this.DelMagicListB.TabIndex = 110;
            // 
            // FindItemResultPlane
            // 
            this.FindItemResultPlane.Controls.Add(this.FindItemResultListB);
            this.FindItemResultPlane.Controls.Add(this.FindItemResult);
            this.FindItemResultPlane.Location = new System.Drawing.Point(0, 467);
            this.FindItemResultPlane.Name = "FindItemResultPlane";
            this.FindItemResultPlane.Size = new System.Drawing.Size(477, 255);
            this.FindItemResultPlane.TabIndex = 113;
            // 
            // FindItemResultListB
            // 
            this.FindItemResultListB.FormattingEnabled = true;
            this.FindItemResultListB.HorizontalScrollbar = true;
            this.FindItemResultListB.ItemHeight = 12;
            this.FindItemResultListB.Location = new System.Drawing.Point(14, 27);
            this.FindItemResultListB.Name = "FindItemResultListB";
            this.FindItemResultListB.Size = new System.Drawing.Size(451, 220);
            this.FindItemResultListB.TabIndex = 111;
            this.FindItemResultListB.SelectedIndexChanged += new System.EventHandler(this.FindItemResultListB_SelectedIndexChanged);
            // 
            // OpenFile
            // 
            this.OpenFile.Location = new System.Drawing.Point(0, -2);
            this.OpenFile.Name = "OpenFile";
            this.OpenFile.Size = new System.Drawing.Size(78, 26);
            this.OpenFile.TabIndex = 114;
            this.OpenFile.Text = "OpenFile";
            this.OpenFile.UseVisualStyleBackColor = true;
            this.OpenFile.Click += new System.EventHandler(this.OpenFile_Click);
            // 
            // About
            // 
            this.About.Location = new System.Drawing.Point(96, -2);
            this.About.Name = "About";
            this.About.Size = new System.Drawing.Size(80, 26);
            this.About.TabIndex = 115;
            this.About.Text = "About";
            this.About.UseVisualStyleBackColor = true;
            this.About.Click += new System.EventHandler(this.About_Click);
            // 
            // CreateItemResultPanel
            // 
            this.CreateItemResultPanel.Controls.Add(this.CreateItemResultListB);
            this.CreateItemResultPanel.Controls.Add(this.CreatItemResultLab);
            this.CreateItemResultPanel.Location = new System.Drawing.Point(483, 467);
            this.CreateItemResultPanel.Name = "CreateItemResultPanel";
            this.CreateItemResultPanel.Size = new System.Drawing.Size(534, 255);
            this.CreateItemResultPanel.TabIndex = 114;
            // 
            // CreateItemResultListB
            // 
            this.CreateItemResultListB.FormattingEnabled = true;
            this.CreateItemResultListB.HorizontalScrollbar = true;
            this.CreateItemResultListB.ItemHeight = 12;
            this.CreateItemResultListB.Location = new System.Drawing.Point(16, 27);
            this.CreateItemResultListB.Name = "CreateItemResultListB";
            this.CreateItemResultListB.Size = new System.Drawing.Size(505, 220);
            this.CreateItemResultListB.TabIndex = 111;
            this.CreateItemResultListB.SelectedIndexChanged += new System.EventHandler(this.CreateItemResultListB_SelectedIndexChanged);
            // 
            // CreatItemResultLab
            // 
            this.CreatItemResultLab.Location = new System.Drawing.Point(3, 10);
            this.CreatItemResultLab.Name = "CreatItemResultLab";
            this.CreatItemResultLab.Size = new System.Drawing.Size(88, 17);
            this.CreatItemResultLab.TabIndex = 37;
            this.CreatItemResultLab.Text = "CreatItemResult";
            // 
            // LevelMinOnlyText1
            // 
            this.LevelMinOnlyText1.Location = new System.Drawing.Point(55, 117);
            this.LevelMinOnlyText1.Name = "LevelMinOnlyText1";
            this.LevelMinOnlyText1.Size = new System.Drawing.Size(37, 21);
            this.LevelMinOnlyText1.TabIndex = 36;
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(93, 120);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(20, 18);
            this.label6.TabIndex = 37;
            this.label6.Text = " - ";
            // 
            // LevelMaxOnlyText1
            // 
            this.LevelMaxOnlyText1.Location = new System.Drawing.Point(119, 117);
            this.LevelMaxOnlyText1.Name = "LevelMaxOnlyText1";
            this.LevelMaxOnlyText1.Size = new System.Drawing.Size(37, 21);
            this.LevelMaxOnlyText1.TabIndex = 38;
            // 
            // Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1016, 734);
            this.Controls.Add(this.CreateItemResultPanel);
            this.Controls.Add(this.About);
            this.Controls.Add(this.OpenFile);
            this.Controls.Add(this.FindItemResultPlane);
            this.Controls.Add(this.MagicPlane);
            this.Controls.Add(this.FindItemPlane);
            this.Controls.Add(this.CreatItemPlane);
            this.Controls.Add(this.ItemInfoPlane);
            this.Name = "Main";
            this.Text = "Main";
            this.ItemInfoPlane.ResumeLayout(false);
            this.ItemInfoPlane.PerformLayout();
            this.CreatItemPlane.ResumeLayout(false);
            this.CreatItemPlane.PerformLayout();
            this.FindItemPlane.ResumeLayout(false);
            this.FindItemPlane.PerformLayout();
            this.MagicPlane.ResumeLayout(false);
            this.MagicPlane.PerformLayout();
            this.FindItemResultPlane.ResumeLayout(false);
            this.CreateItemResultPanel.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
        private System.Windows.Forms.Label IDMoreLab;
        private System.Windows.Forms.TextBox IDMoreMinText;
        private System.Windows.Forms.TextBox IDMoreMaxText;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label MainTypeLab1;
        private System.Windows.Forms.ComboBox MainTypeComB1;
        private System.Windows.Forms.ComboBox SubTypeComB1;
        private System.Windows.Forms.Label SubTypeLab1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox LevelMoreMaxText;
        private System.Windows.Forms.TextBox LevelMoreMinText;
        private System.Windows.Forms.Label LevelMoreLab;
        private System.Windows.Forms.ComboBox QualityComb1;
        private System.Windows.Forms.Label QualityLab1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox PriceMoreMaxText;
        private System.Windows.Forms.TextBox PriceMoreMinText;
        private System.Windows.Forms.Label PriceMoreLab;
        private System.Windows.Forms.ComboBox LeashComB1;
        private System.Windows.Forms.Label LeashLab1;
        private System.Windows.Forms.TextBox SetIDText1;
        private System.Windows.Forms.Label SetIDLab1;
        private System.Windows.Forms.Button FindItem;
        private System.Windows.Forms.Button CreateItem;
        private System.Windows.Forms.ComboBox SubTypeComb2;
        private System.Windows.Forms.Label SubTypeLab2;
        private System.Windows.Forms.ComboBox MainTypeComB2;
        private System.Windows.Forms.Label TypeLab2;
        private System.Windows.Forms.ComboBox SchoolComb1;
        private System.Windows.Forms.Label SchoolLab1;
        private System.Windows.Forms.ComboBox QualityComb2;
        private System.Windows.Forms.Label QualityLab2;
        private System.Windows.Forms.Label LevelOnlyLab1;
        private System.Windows.Forms.Label FindItemResult;
        private System.Windows.Forms.Label AddMagicLab;
        private System.Windows.Forms.Button AddMagicBut;
        private System.Windows.Forms.Button DelMagicBut;
        private System.Windows.Forms.Label DelMagicLab;
        private System.Windows.Forms.Panel ItemInfoPlane;
        private System.Windows.Forms.Label DemValueLab6;
        private System.Windows.Forms.Label DemValueLab5;
        private System.Windows.Forms.Label DemValueLab4;
        private System.Windows.Forms.Label DemValueLab3;
        private System.Windows.Forms.Label DemValueLab2;
        private System.Windows.Forms.Label DemValueLab1;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox AttributeMaxText2;
        private System.Windows.Forms.TextBox AttributeMinText2;
        private System.Windows.Forms.ComboBox AttributeComb2;
        private System.Windows.Forms.Label AttributeLab2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox AttributeMaxText1;
        private System.Windows.Forms.TextBox AttributeMinText1;
        private System.Windows.Forms.ComboBox AttributeComb1;
        private System.Windows.Forms.Label AttributeLab1;
        private System.Windows.Forms.TextBox NameText;
        private System.Windows.Forms.Label NameLab;
        private System.Windows.Forms.TextBox SetIDText2;
        private System.Windows.Forms.Label SetIDLab2;
        private System.Windows.Forms.TextBox RepIDText;
        private System.Windows.Forms.Label RepIDLab;
        private System.Windows.Forms.ComboBox SchoolComb2;
        private System.Windows.Forms.Label SchoolLab2;
        private System.Windows.Forms.TextBox UiIDText;
        private System.Windows.Forms.Label UiIDLab;
        private System.Windows.Forms.ComboBox LeashComb2;
        private System.Windows.Forms.Label LeashLab2;
        private System.Windows.Forms.TextBox PriceOnlyText2;
        private System.Windows.Forms.Label PriceOnlyLab2;
        private System.Windows.Forms.ComboBox QualityComb3;
        private System.Windows.Forms.Label QualityLab3;
        private System.Windows.Forms.TextBox LevelOnlyText2;
        private System.Windows.Forms.Label LevelOnlyLab2;
        private System.Windows.Forms.ComboBox SubTypeComb3;
        private System.Windows.Forms.Label SubTypeLab3;
        private System.Windows.Forms.ComboBox MainTypeComB3;
        private System.Windows.Forms.Label TypeLab3;
        private System.Windows.Forms.TextBox IDOnlyText;
        private System.Windows.Forms.Label IDOnlyLab;
        private System.Windows.Forms.Label ItemInfoLab;
        private System.Windows.Forms.ListBox AddMagicListb;
        private System.Windows.Forms.Panel CreatItemPlane;
        private System.Windows.Forms.Panel FindItemPlane;
        private System.Windows.Forms.Panel MagicPlane;
        private System.Windows.Forms.ListBox DelMagicListB;
        private System.Windows.Forms.Panel FindItemResultPlane;
        private System.Windows.Forms.ListBox FindItemResultListB;
        private System.Windows.Forms.TextBox DemValueText6;
        private System.Windows.Forms.TextBox DemValueText5;
        private System.Windows.Forms.TextBox DemValueText4;
        private System.Windows.Forms.TextBox DemValueText3;
        private System.Windows.Forms.TextBox DemValueText2;
        private System.Windows.Forms.TextBox DemValueText1;
        private System.Windows.Forms.Button OpenFile;
        private System.Windows.Forms.Button About;
        private System.Windows.Forms.Panel CreateItemResultPanel;
        private System.Windows.Forms.ListBox CreateItemResultListB;
        private System.Windows.Forms.Label CreatItemResultLab;
        private System.Windows.Forms.Label ItemValueLab;
        private System.Windows.Forms.Button AddItem;
        private System.Windows.Forms.Button DelItem;
        private System.Windows.Forms.TextBox DemValueText8;
        private System.Windows.Forms.Label DemValueLab8;
        private System.Windows.Forms.TextBox DemValueText7;
        private System.Windows.Forms.Label DemValueLab7;
        private System.Windows.Forms.Label ItemTotalValueLab;
        private System.Windows.Forms.Label RemainMagicValue;
        private System.Windows.Forms.TextBox ItemValueText;        

        public const int MAX_ITEM_INFO_SIZE = 4096;
        public string m_strDelimStr = "\t";
        public string[] m_strSubTypeByPrgm = new string[] { "武器", "远程武器", "上衣", "头部", "戒指", "项链", "腰带", "腰缀", "裤子", "鞋子", "护臂", "扩展类", "包裹" };

        public int[] m_nFileInfoNumber = new int[(int)FilePath.FILE_PATH_END];
        public string[] m_strFile = new string[(int)FilePath.FILE_PATH_END];
        public string[] m_strFileHead = new string[(int)FilePath.FILE_PATH_END];
        public string[] m_strTypeName = new string[(int)MainTypeName.MAIN_TYPE_NAME_END];

        public int[] m_nCuston_Info_Count = new int[(int)MainTypeName.MAIN_ITEM_TYPE_END - (int)MainTypeName.MAIN_ITEM_TYPE_BEGIN];
        public string[,] m_Custon_Info = new string[(int)MainTypeName.MAIN_ITEM_TYPE_END - (int)MainTypeName.MAIN_ITEM_TYPE_BEGIN, MAX_ITEM_INFO_SIZE];
        public Custon_ItemInfo[,] m_Custon_Item = new Custon_ItemInfo[(int)MainTypeName.MAIN_ITEM_TYPE_END - (int)MainTypeName.MAIN_ITEM_TYPE_BEGIN, MAX_ITEM_INFO_SIZE];

        public int[] m_nCH_Info_Count = new int[(int)MainTypeName.MAIN_ITEM_TYPE_END - (int)MainTypeName.MAIN_ITEM_TYPE_BEGIN];
        public string[,] m_CH_Info = new string[(int)MainTypeName.MAIN_ITEM_TYPE_END - (int)MainTypeName.MAIN_ITEM_TYPE_BEGIN, MAX_ITEM_INFO_SIZE];
        public CH_ItemInfo[,] m_CH_Item = new CH_ItemInfo[(int)MainTypeName.MAIN_ITEM_TYPE_END - (int)MainTypeName.MAIN_ITEM_TYPE_BEGIN, MAX_ITEM_INFO_SIZE];

        public string[] m_strSchoolTable = new string[(int)MainTypeName.SCHOOL_END - (int)MainTypeName.SCHOOL_BEGIN];
        public string[] m_strMainTypeTable = new string[(int)MainTypeName.MAIN_ITEM_TYPE_END - (int)MainTypeName.MAIN_ITEM_TYPE_BEGIN];
        public string[] m_strSubTypeTable = new string[(int)MainTypeName.SUB_ITEM_TYPE_END - (int)MainTypeName.SUB_ITEM_TYPE_BEGIN];

        public int[,] m_nValue = new int[100, 4];
        public string[,] m_strMagicTypeName = new string[100, 3];
        public string[,] m_strMagicTypeValue = new string[398, 20];

        public double[,] m_fWGHurt = new double[100, 7];
        public double[,] m_fWGDefend = new double[100, 6];

        public string[,] m_strItemArmorCom = new string[14, 5];
        public string[,] m_strItemTrinketCom = new string[14, 8];
        public string[,] m_strItemWeaponCom = new string[14, 7];

        public int m_nItemIndex = -1;
        public int m_nItemEntryCount = 0;
        public int m_nCreateLevelMin = 0;
        public int m_nCreateLevelMax = 0;
        public int m_nSchoolRowOfCom = 0;
        public string[] m_strCreateCondition = new string[6];
        public ItemEntry[] m_ItemEntry = new ItemEntry[MAX_ITEM_INFO_SIZE];

        public int[,] m_nMagicItemBeginToEnd = { {98, 197}, {198, 297}, {298, 397}};
        public double[] m_fQuality = { 1.0, 1.2, 1.4, 1.6 };
        public double[] m_fWGHurtBase = { 1.0, 1.2, 1.1, 1.0, 1.0, 1.1};
        public double[] m_fWGHurtDrift = { 0.2, 0.2, 0.1, 0.1, 0.1, 0.1};
        public double[] m_fWGDefendBase = { 0.225, 0.175, 0.15, 0.2, 0.125, 0.175};
        public double[] m_fMagicBaseAr = { 1.0, 0.8, 0.9, 0.5, 0.5, 0.7, 
                                           1.0, 1.0, 1.0, 
                                           1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
        private System.Windows.Forms.TextBox LevelMaxOnlyText1;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox LevelMinOnlyText1;
    }
}

