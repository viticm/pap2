using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Collections;
using System.Windows.Forms;

namespace GameDesingerTools
{
    public enum enumValueType { vtUnknown, vtText, vtCombo, vtExpandNode, vtBool, vtPath, vtFileName, vtFileNameScript, vtFileNameSfx, vtCustomEditor };
    public class CombxHelper
    {
        private Hashtable m_hs;
        private CustomProperty m_prop;
        public CombxHelper(CustomProperty prop)
        {
            m_prop = prop;
        }
        public Hashtable Values
        {
            set { m_hs = value; }
        }
        
        public string this[string astrkey]
        {
            get {
                string strkey = astrkey.Trim().ToLower();

                if (m_hs != null && m_prop.ComboValues[m_prop.Value] != null && m_hs[m_prop.ComboValues[m_prop.Value]] != null)
                {
                    Hashtable hs = (Hashtable)m_hs[m_prop.ComboValues[m_prop.Value]];
                    if (hs[strkey] != null)
                        return hs[strkey].ToString();
                }
                return null;
            }
        }
    };
    /// <summary>
    /// Custom property class 
    /// </summary>
    public class CustomProperty
    {
        private string sName = string.Empty;
        private string m_strCategory = string.Empty;
        private bool bReadOnly = false;
        private bool bVisible = true;
        private object m_objValue = null;
        private string m_strDisplayValue = null;
        private object defobjValue = null;
        private bool m_bValid = true;
        private string m_sDisplayName = string.Empty;
        private string m_sDescription = string.Empty;
        private int m_nOrder = 0;
        private enumValueType m_vt = enumValueType.vtText;
        Hashtable m_values = null;
        Hashtable m_combokeys = null;
        Hashtable m_combofieldex = null;
        CombxHelper m_combofieldhelper = null;
        TypeConverter.StandardValuesCollection m_CollectionValue = null;
        private string m_strid;
        private string m_strPrefix, m_strCutPrefix;
        private string m_strkey;
        protected object m_parent;
        private object m_objTag;
        private bool m_bCanHideNode = false;
        private bool m_bCutSlash = true;

        public CustomProperty(CustomClass parent, string strID, string sName, object value, bool bReadOnly, bool bVisible, enumValueType vtType)
        {
            m_combofieldhelper = new CombxHelper(this);
            
            m_parent = parent;
            this.m_strid = strID;
            this.sName = sName;
            this.m_objValue = value;
            this.bReadOnly = bReadOnly;
            this.bVisible = bVisible;
            this.m_vt = vtType;
        }
        public object Parent
        {
            get { return m_parent; }
        }
        public string ID
        {
            get { return m_strid; }
            set { m_strid = value; }
        }

        public string Key
        {
            get { return m_strkey; }
            set { m_strkey = value; }
        }
        
        public enumValueType ValueType
        {
            get { return m_vt; }
            set { m_vt = value; }
        }

        public string DisplayValue
        {
            get { return m_strDisplayValue; }
            set { m_strDisplayValue = value.ToString();
                if (this.Value is CustomClass)
                {
                    (this.Value as CustomClass).DisplayValue = m_strDisplayValue;
                }
            }
        }

        public string Prefix
        {
            get { return m_strPrefix; }
            set { m_strPrefix = value; }
        }
        public string CutPrefix
        {
            get { return m_strCutPrefix; }
            set { m_strCutPrefix = value; }
        }
        public bool CutPreSlash
        {
            get { return m_bCutSlash; }
            set { m_bCutSlash = value; }
        }
        public bool IsCanHideNode
        {
            get { return m_bCanHideNode; }
            set { m_bCanHideNode = value; }
        }
        #region "下拉框相关"
        public Hashtable ComboValues //key=displaytext, value=id
        {
            get { return m_values; }
            set { m_values = value; }
        }
        public Hashtable ComboKeys   //key=id, value=displaytext
        {
            get { return m_combokeys; }
            set { m_combokeys = value; }
        }

        public Hashtable ComboValuesEx
        {
            set { m_combofieldex = value; m_combofieldhelper.Values = m_combofieldex; }
            get { return m_combofieldex; }
        }
        public CombxHelper Field
        {
            get {
                if (m_combofieldhelper != null)
                {

                    return m_combofieldhelper;
                }
                else
                    return null;
            }
        }

        public bool ShouldSerializeValue()
        {
            /*
            if (defobjValue == null)// && m_objValue == null)
                return false;
            else
            */ 
            {
                if (m_objValue is DBCustomClass)
                {
                    DBCustomClass clsSrc = (DBCustomClass)m_objValue;
                    if (clsSrc.IsVirtualField)
                        return false;
                    else
                        return !clsSrc.Value.Equals(defobjValue);
                }
                else if (m_objValue != null)
                {
                    return !m_objValue.Equals(defobjValue);
                }
                else
                {
                    return false;
                }
            }
        }

        public bool SetValues(string strValues)
        {
            string strlistvalues = strValues;
            string[] strLines = Helper.split(strlistvalues, "\r\n");
            
            Hashtable comhashkeys = new Hashtable();
            Hashtable comhashvalues = new Hashtable();
            foreach (string line in strLines)
            {
                string[] strkv = line.Split(new Char[] {','});
                if (strkv != null && strkv.Length > 1)
                {
                    string key = strkv[0];
                    string value = strkv[1];
                    comhashkeys[value] = key;
                    comhashvalues[key] = value;
                }
                else
                {
                    string s = string.Format("在设置 {0} 字段 ListValues 内容时出现错误，请修改SetValues的调用参数为正确的格式。", ID);
                    System.Windows.Forms.MessageBox.Show(s);
                    throw new Exception("错误的ListValues");
                }
            }
            m_combokeys = comhashkeys;
            m_values = comhashvalues;
            return true;
        }
        //转为下拉框

        public TypeConverter.StandardValuesCollection GetValues()
        {
            if (m_CollectionValue == null)
            {
                int ncount = 0;
                if (ComboValues == null || ComboValues.Count <= 0)
                    return null;

                string[] comvalue = new string[ComboValues.Count];
                
                ArrayList a_combovalues = new ArrayList(ComboValues);
                StringComparer comboComp = new StringComparer();
                a_combovalues.Sort(comboComp);

                foreach (DictionaryEntry kv in /*ComboValues*/a_combovalues)
                {
                    comvalue[ncount++] = (string)kv.Key;
                }

                m_CollectionValue = new TypeConverter.StandardValuesCollection(comvalue);
            }
            return m_CollectionValue;
        }
        #endregion

        public bool CausesValidation
        {
            get
            {
                return m_bValid;
            }
            set
            {
                m_bValid = value;
            }
        }
        public string Category
        {
            get
            {
                return m_strCategory;
            }
            set
            {
                m_strCategory = value;
            }
        }
        public int OrderNo
        {
            get
            {
                return m_nOrder;
            }
            set
            {
                m_nOrder = value;
            }
        }
        public bool ReadOnly
        {
            get
            {
                return bReadOnly;
            }
            set
            {
                bReadOnly = value;
            }
        }
        public string Description
        {
            get
            {
                return m_sDescription;
            }
            set
            {
                m_sDescription = value;
            }
        }
        public string DisplayName
        {
            get
            {
                return m_sDisplayName;
            }
            set
            {
                m_sDisplayName = value;
            }
        }
        public string Name
        {
            get
            {
                return sName;
            }
            set { sName = value; }
        }
        public bool Visible
        {
            get
            {
                return bVisible;
            }
            set
            {
                bVisible = value;
            }
        }
        public object Value
        {
            get
            {
                return m_objValue;
            }
            set
            {
                // 标记值改变
                DBCustomClass dbCustomClass = m_parent as DBCustomClass;

                if (dbCustomClass != null && dbCustomClass.InitFinished)
                {
                    dbCustomClass.MarkValueChanged();
                }

                DBCustomClass grandParent = dbCustomClass.Parent as DBCustomClass;
                if (grandParent != null && grandParent.IsVirtualField && grandParent.InitFinished)
                {
                    dbCustomClass.MarkValueChanged();
                }

                m_objValue = value;                
            }
        }
        public object DefaultValue
        {
            get { return defobjValue;}
            set { 
                if (value != null && value is CustomClass)
                    defobjValue = (value as CustomClass).Value; 
                else
                    defobjValue = value; 
            }
        }
        public void SetRealValue(object value)
        {
            SetValue(value);
            if (Parent != null && Parent is DBCustomClass)
            {
                DBCustomClass parentClass = (DBCustomClass)Parent;
                parentClass.OnAsynNotifyEvent(2, this);
            }            
        }
        //只有在树结点中输入新值，才会调用这里
        public void SetValue(object value)
        {
            if (value == Value)
                return;

            if (Parent != null && Parent is CustomClass)
            {
                CustomClass o = (CustomClass)Parent;
                if (ValueType == enumValueType.vtCombo)
                    o.OnDataValidate(this, ComboValues[value], value);
                else
                    o.OnDataValidate(this, value, value);
            }

            if (Parent != null && Parent is DBCustomClass)
            {
                ((DBCustomClass)Parent).MarkValueChanged();
            }

            bool bRealField = false;
            if (m_objValue is DBCustomClass) //如果是树,则需要影响下面的结点,而不是简单替换值
            {
                DBCustomClass cls = (DBCustomClass)m_objValue;
                bRealField = cls.ParentFieldType != 1;
                //cls.Value = value;
                //cls.GetDynamicClass(new object[] { value }); //不能直接使用value,因可能是修改的非主键字段,要获取这个真正的主键
                if (bRealField)
                    cls.ReplaceSubFindKey(this, value);
                cls.OnAsynNotifyEvent(1, null);//todo:多个页存在同一表同一字段时，会导致重复刷新多次
            }
            else
            {
                m_objValue = value;
                bRealField = true;
            }
            //如果改了主键值，则需要特殊处理
            if (Parent != null && Parent is DBCustomClass && bRealField)
            {
                DBCustomClass parentClass = (DBCustomClass)Parent;
                //parentClass.DBPrimaryKey
                parentClass.ModifyFindKeys(this, value);
            }
            if (Parent != null && Parent is CustomClass)
            {
                CustomClass o = (CustomClass)Parent;
                if (ValueType == enumValueType.vtCombo)
                    o.OnAfterDataValidate(this, ComboValues[value], value);
                else
                    o.OnAfterDataValidate(this, value, value);
            }
        }
        public object Tag
        {
            get
            {
                return m_objTag;
            }
            set
            {
                m_objTag = value;
            }
        }
        
        public CustomProperty this[string strID]
        {
            get
            {
                if (this.m_objValue is CustomClass)
                {
                    return ((CustomClass)(m_objValue))[strID];
                }                
                return null;
            }
        }

	    public CustomProperty this[int index] 
	    {
		    get 
		    {
                if (this.m_objValue is CustomClass)
                {
                    return ((CustomClass)(m_objValue))[index];
                }
                return null;

		    }
		    set
		    {
                if (this.m_objValue is CustomClass)
                {
                    ((CustomClass)(m_objValue))[index] = (CustomProperty)value;
                }
		    }
        }
        public int Count
        {
            get{
                if (this.m_objValue is CustomClass)
                {
                    return ((CustomClass)(m_objValue)).Count;
                }
                return 0;
            }
        }

    }
    /// <summary>
    /// Custom PropertyDescriptor
    /// </summary>
    public class CustomPropertyDescriptor : PropertyDescriptor
    {
        CustomProperty m_Property;
        public CustomPropertyDescriptor(ref CustomProperty myProperty, Attribute[] attrs)
            : base(myProperty.Name, attrs)
        {
            m_Property = myProperty;
        }
        public CustomProperty Property
        {
            get { return m_Property; }
        }

        #region PropertyDescriptor 实现

        public override void ResetValue(object component)
        {
            //SetValue(component, "1");
        }

        public override bool CanResetValue(object component)
        {
            //return true;
            return false;
        }
        public override bool ShouldSerializeValue(object component)
        {
            return m_Property.ShouldSerializeValue();/*
            if (m_Property.DefaultValue == null && m_Property.Value == null)
                return false;
            else
            {
                if (m_Property.Value is CustomClass)
                {
                    CustomClass clsSrc = (CustomClass)m_Property.Value;
                    return !clsSrc.Value.Equals(m_Property.DefaultValue);
                }
                else
                    return !m_Property.Value.Equals(m_Property.DefaultValue);
            }
            */
        }

        public override Type ComponentType
        {
            get
            {
                return null;
            }
        }

        public override object GetValue(object component) //用于获取显示内容
        {
            if (m_Property.DisplayValue != null && !(m_Property.Value is CustomClass))
                return m_Property.DisplayValue;
            else
                return m_Property.Value;
        }

        public override string Description
        {
            get
            {
                if (m_Property.Description == string.Empty)
                    return m_Property.Name;
                else
                    return m_Property.Description;
            }
        }
        /*public override bool IsBrowsable //不知道为什么没有用处，在GetProperties中进行处理了
        {
            get { return false; }
        }*/

        public override string Category
        {
            get
            {
                return m_Property.Category;//string.Empty ;
            }
        }

        public override string DisplayName
        {
            get
            {
                return m_Property.Name;// +"中文名";
            }

        }

        public override bool IsReadOnly
        {
            get
            {
                return m_Property.ReadOnly;
            }
        }

        public override void SetValue(object component, object value)
        {
            try
            {
                SetValueCommand cmd = new SetValueCommand();
                cmd.Execute();
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message, "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                throw;
            }

            if (component is CustomClass)
            {
                CustomClass o = (CustomClass)component;
                if (m_Property.ValueType == enumValueType.vtCombo)
                    o.OnDataValidate(m_Property, m_Property.ComboValues[value], value);
                else
                    o.OnDataValidate(m_Property, value, value);
            }

            if (m_Property.ValueType == enumValueType.vtFileNameScript)
            {
                string strFullPath = (string)value;

                // to 相对路径
                string strBase = null;
                if (System.IO.File.Exists(Application.StartupPath + "\\GameDesingerTools.ini"))
                {
                    StringBuilder sb = new StringBuilder(255);
                    FileFolderHelper.ReadIniFile("Browsing Base Folder", "Script", "", sb, 255, Application.StartupPath + "\\GameDesingerTools.ini");
                    strBase = sb.ToString();
                }
                string strConverted = strFullPath.Replace(strBase + "\\", "");

                // '\' -> '/'
                strConverted = strConverted.Replace("\\", "/");
                m_Property.Value = strConverted;
            }
            else if (m_Property.ValueType == enumValueType.vtExpandNode && m_Property.Value is DBCustomClass)
            {
                //((DBCustomClass)m_Property.Value).Value = value;
                //((DBCustomClass)m_Property.Value).ReinitData();
                //((DBCustomClass)m_Property.Value).GetDynamicClass(new object[]{value});
                //m_Property.Value = (DBCustomClass)m_Property.Value;
                //m_Property.ValueType = enumValueType.vtExpandNode;
            }
            else
            {
                //if (m_Property.IsClearDisplayValue) //仅在 oneditvalue 时会被置 false, 用于设置显示值，不影响保存的值
                //    m_Property.DisplayValue = null;
                //m_Property.IsClearDisplayValue = true;
                m_Property.Value = value;
            }

            if (component is CustomClass)
            {
                CustomClass o = (CustomClass)component;
                if (m_Property.ValueType == enumValueType.vtCombo)
                    o.OnAfterDataValidate(m_Property, m_Property.ComboValues[value], value);
                else
                    o.OnAfterDataValidate(m_Property, value, value);
            }
        }

        public override Type PropertyType
        {
            get
            {
                if (m_Property.Value != null)
                    return m_Property.Value.GetType();
                else
                    return null;
            }
            //get { return typeof(CollectionBase);} //将editor变为增加Collection

        }

        public override object GetEditor(Type editorBaseType)
        {
            switch (m_Property.ValueType)
            {
                case enumValueType.vtFileName:
                    return new FileNameEditorEx();// System.Windows.Forms.Design.FileNameEditor();

                case enumValueType.vtFileNameSfx:
                    return new System.Windows.Forms.Design.FileNameEditor();

                case enumValueType.vtFileNameScript:
                    return new System.Windows.Forms.Design.FileNameEditor();

                case enumValueType.vtPath:
                    return new System.Windows.Forms.Design.FolderNameEditor();

                case enumValueType.vtCustomEditor:
                    return new CustomEditor();

                case enumValueType.vtBool:
                    return new BoolEditor();                    
                default:
                    return base.GetEditor(editorBaseType);
            }
        }

        public override TypeConverter Converter
        {
            get
            {
                //return base.Converter;
                //return new TreeConverter();//转为可展开的节点                
                switch (m_Property.ValueType)
                {
                    case enumValueType.vtCombo:
                        return new ComboConverter(m_Property); //转为下拉框 
                        //break;
                    case enumValueType.vtBool:
                        return new BoolConverter(m_Property);
                        //break;
                    case enumValueType.vtExpandNode:
                        return new TreeConverter(m_Property);
                    default:
                        return base.Converter;
                }
            }
        }
        #endregion
    }
    #region "Convert Implement"
    internal class BoolConverter : TypeConverter
    {
        CustomProperty m_property;
        public BoolConverter(CustomProperty aproperty)
        {
            m_property = aproperty;
        }
        public override StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
        {
            return new TypeConverter.StandardValuesCollection(new string[] { "是", "否" });
        }
        public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
        {
            return true;
        }
        public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
        {
            return true;
        }
    }

    internal class ComboConverter : StringConverter
    {
        CustomProperty m_property;
        public ComboConverter(CustomProperty aproperty)
        {
            m_property = aproperty;
        }
        public override bool GetStandardValuesSupported(ITypeDescriptorContext context)
        {
            return true;
        }
        public override bool GetStandardValuesExclusive(ITypeDescriptorContext context)
        {
            return true; //是否只能设置为下拉列表中的值
        }
        public override StandardValuesCollection GetStandardValues(ITypeDescriptorContext context)
        {
            //return base.GetStandardValues(context);
            return m_property.GetValues();
        }
    }

    internal class TreeConverter : ExpandableObjectConverter
    {
        CustomProperty m_property;
        public TreeConverter(CustomProperty aproperty)
        {
            m_property = aproperty;
        }

        public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
        {
            if (context.Instance is CustomClass)
            {
                CustomPropertyDescriptor desc = (CustomPropertyDescriptor)context.PropertyDescriptor;
                if (desc.Property.Value is CustomClass)
                {
                    desc.Property.SetValue(value);
                    /*
                    DBCustomClass cls = (DBCustomClass)desc.Property.Value;
                    cls.Value = value;
                    cls.GetDynamicClass(new object[]{value});
                    while (cls.Parent != null)
                    {
                        if (cls.Parent is DBCustomClass)
                        {
                            cls = (DBCustomClass)(cls.Parent);
                        }
                    }
                    cls.OnAsynNotifyEvent(1);
                    */
                    return desc.Property.Value;
                }
            }
            return base.ConvertFrom(context, culture, value);
        }
        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            if (sourceType == typeof(string) && context.Instance is CustomClass)
            {
                CustomPropertyDescriptor desc = (CustomPropertyDescriptor)context.PropertyDescriptor;
                if (desc.Property.Value is CustomClass)
                {
                    DBCustomClass cls = ((DBCustomClass)(desc.Property.Value));
                    if (!cls.IsVirtualField) //虚拟字段本身不能修改值
                        return true;
                }
            }
            return base.CanConvertFrom(context, sourceType);            
        }
        
        public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destType)
        {
            if (destType == typeof(string) && context.Instance is CustomClass)
            {
                CustomPropertyDescriptor desc = (CustomPropertyDescriptor)context.PropertyDescriptor;
                if (desc.Property.Value is CustomClass)
                {
                    CustomClass cls = (CustomClass)context.Instance;
                    object ovalue = null;
                    try
                    {
                        CustomClass childclass = (CustomClass)desc.Property.Value;
                        //string strdesc = childclass.Value == null ? "" : childclass.Value;
                        //ovalue = cls.OnCustomGetTreeValue(childclass, strdesc);
                        ovalue = cls.OnCustomGetTreeValue((CustomProperty)desc.Property);//, strdesc);
                        if (ovalue == null)
                        {
                            if (childclass.DisplayValue == null)
                                return childclass.Value.ToString();
                            else
                                return childclass.DisplayValue;
                        }
                        else
                            return ovalue.ToString();
                        //else
                        //    return "...";
                    }
                    catch
                    {

                    }


                    /*CustomClass cls = (CustomClass)desc.Property.Value;
                    string strValue = "";
                    foreach(CustomProperty prop in cls)
                    {
                        strValue = strValue + prop.ID + " = " + prop.Value + " ";
                    }
                    */                    
                    //return ((CustomClass)desc.Property.Value).Description;
                }
                
            }
            return base.ConvertTo(context, culture, value, destType);
        }
    }
    #endregion
}
