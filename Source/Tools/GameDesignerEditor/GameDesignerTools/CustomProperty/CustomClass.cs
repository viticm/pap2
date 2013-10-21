using System;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms.Design;

namespace GameDesingerTools
{
    public class PropertyChangeEventArgs : EventArgs
    {
        public PropertyChangeEventArgs(object oldid, object oldvalue, object newid, object newvalue)
        {
            m_oldvalue = oldvalue;
            m_newvalue = newvalue;
            m_oldid = oldid;
            m_newid = newid;
        }
        private object m_oldvalue;
        private object m_newvalue;
        private object m_oldid;
        private object m_newid;

        public object oldvalue { get { return m_oldvalue; } }
        public object newvalue { get { return m_newvalue; } }
        public object oldid { get { return m_oldid; } }
        public object newid { get { return m_newid; } }
    }
    public class PropertyAfterChangeEventArgs : EventArgs
    {
        public PropertyAfterChangeEventArgs(object id, object value)
        {
            m_value = value;
            m_id = id;
        }
        private object m_value;
        private object m_id;

        public object value { get { return m_value; } }
        public object id { get { return m_id; } }
    }

    public class CustomEditorEventArgs : EventArgs
    {
        public CustomEditorEventArgs(object id, object value)
        {
            m_value = value;
            m_id = id;
        }
        private object m_value;
        private object m_id;

        public object value { get { return m_value; } }
        public object id { get { return m_id; } }
    }
    public class TreeValueEventArgs : EventArgs
    {
        public TreeValueEventArgs(CustomClass Parent, CustomProperty Child)//, object id, object value)
        {
            m_parentclass = Parent;
            m_childclass = Child;
            //m_id = id;
            //m_class = Parent;
        }
        //private object m_value;
        //private object m_id;
        private CustomClass m_parentclass;
        private CustomProperty m_childclass;

        //public object value { get { return m_value; } }
        //public object id { get { return m_id; } }
        public CustomClass parent { get { return m_parentclass; } }
        public CustomProperty tree { get { return m_childclass; } }
    }

    public class CustomClass: CollectionBase,ICustomTypeDescriptor
	{
        private string m_strDesc;
        private string m_strID;
        #region "事件实现"
        public delegate void PropertyChangeEventHandler(CustomProperty sender, PropertyChangeEventArgs e);
        public delegate void PropertyAfterChangeEventHandler(CustomProperty sender, PropertyAfterChangeEventArgs e);
        public event PropertyChangeEventHandler DataValidate;
        public event PropertyAfterChangeEventHandler AfterDataValidate;

        public delegate object CustomEditValueEventHandler(CustomProperty sender, CustomEditorEventArgs e);
        public event CustomEditValueEventHandler OnEditValue;

        public delegate object TreeValueEventHandler(CustomClass sender, TreeValueEventArgs e);
        public event TreeValueEventHandler OnGetTreeValue;
        private string m_strDisplayValue = null;
        object m_ovalue, m_odefaultvalue; //仅用于树结点时，应该显示的内容
        
        public object DefaultValue
        {
            get { return m_odefaultvalue; }
            set { m_odefaultvalue = value; }
        }
        public object Value
        {
            get { return m_ovalue; }
            set { m_ovalue = value; }
        }
        public string DisplayValue
        {
            get { return m_strDisplayValue; }
            set{m_strDisplayValue = value;}
        }
        public object OnCustomGetTreeValue(CustomProperty prop)//, object Value)
        {
            if (OnGetTreeValue != null)
            {
                return OnGetTreeValue(this, new TreeValueEventArgs(this, prop));
            }          
            return Value;
        }

        public object OnCustomEditValue(CustomProperty prop, object Value)
        {
            if (OnEditValue != null)
            {
                return OnEditValue(prop, new CustomEditorEventArgs(prop.ID, Value));
            }
            return Value;
        }

        public bool OnDataValidate(CustomProperty prop, object ID, object Value)
        {
            if (DataValidate != null)
            {
                if (prop.ValueType == enumValueType.vtCombo)
                    DataValidate(prop, new PropertyChangeEventArgs(prop.ComboValues[prop.Value], prop.Value, ID, Value));
                else
                    DataValidate(prop, new PropertyChangeEventArgs(prop.Value, prop.Value, ID, Value));
            }
            return true;
        }
        public bool OnAfterDataValidate(CustomProperty prop, object ID, object Value)
        {
            if (AfterDataValidate != null)
            {
                AfterDataValidate(prop, new PropertyAfterChangeEventArgs(ID, Value));
            }
            return true;
        }

        // 设置事件响应到每一个属性中 modify by suntao
        public void SetEvents(Hashtable events)
        {
            DataValidate -= events["PropertyChangeEventHandler"] as PropertyChangeEventHandler;
            DataValidate += events["PropertyChangeEventHandler"] as PropertyChangeEventHandler;
            AfterDataValidate -= events["PropertyAfterChangeEventHandler"] as PropertyAfterChangeEventHandler;
            AfterDataValidate += events["PropertyAfterChangeEventHandler"] as PropertyAfterChangeEventHandler;
            OnEditValue -= events["CustomEditValueEventHandler"] as CustomEditValueEventHandler;           
            OnEditValue += events["CustomEditValueEventHandler"] as CustomEditValueEventHandler;
            OnGetTreeValue -= events["TreeValueEventHandler"] as TreeValueEventHandler; 
            OnGetTreeValue += events["TreeValueEventHandler"] as TreeValueEventHandler;            
        }

        #endregion

        public void Add(CustomProperty Value)
		{
			base.List.Add(Value);
		}
		public void Remove(string Name)
		{
			foreach(CustomProperty prop in base.List)
			{
				if(prop.Name == Name)
				{
					base.List.Remove(prop);
					return;
				}
			}
		}

        public CustomProperty this[string strID]
        {
            get
            {
                string strtemp = strID.Trim().ToLower();
                string strid = strtemp;
                bool bFindSubNode = strID.IndexOf('.') >= 0;
                if (bFindSubNode)
                    strid = strtemp.Substring(0, strID.IndexOf('.'));

                foreach (CustomProperty prop in base.List)
                {
                    if (prop.ID.ToLower() == strid)// || strid == '*') //可以忽略所有的中间字段
                    {
                        if (prop.ValueType == enumValueType.vtExpandNode && bFindSubNode)
                        {
                            return ((CustomClass)(prop.Value))[strtemp.Substring(strID.IndexOf('.') + 1)];
                        }
                        else
                            return prop;
                    }
                } 
                return null;
            }
        }

		public CustomProperty this[int index] 
		{
			get 
			{
				return (CustomProperty)base.List[index];
			}
			set
			{
				base.List[index] = (CustomProperty)value;
			}
		}
        public string Description { get { return m_strDesc; } set { m_strDesc = value; } }
        public string ID { get { return m_strID; } set { m_strID = value; } }

		#region "TypeDescriptor Implementation"
		public String GetClassName()
		{
			return TypeDescriptor.GetClassName(this,true);
		}
		public AttributeCollection GetAttributes()
		{
			return TypeDescriptor.GetAttributes(this,true);
		}
		public String GetComponentName()
		{
			return TypeDescriptor.GetComponentName(this, true);
		}
		public TypeConverter GetConverter()
		{
   			return TypeDescriptor.GetConverter(this, true);
		}
		public EventDescriptor GetDefaultEvent() 
		{
			return TypeDescriptor.GetDefaultEvent(this, true);
		}
		public PropertyDescriptor GetDefaultProperty() 
		{
			return TypeDescriptor.GetDefaultProperty(this, true);
		}
		public object GetEditor(Type editorBaseType) 
		{
			return TypeDescriptor.GetEditor(this, editorBaseType, true);
		}

		public EventDescriptorCollection GetEvents(Attribute[] attributes) 
		{
			return TypeDescriptor.GetEvents(this, attributes, true);
		}

		public EventDescriptorCollection GetEvents()
		{
			return TypeDescriptor.GetEvents(this, true);
		}

        public PropertyDescriptorCollection GetProperties(Attribute[] attributes)
		{
            CustomProperty prop = null;
            PropertyDescriptor[] newProps = new PropertyDescriptor[this.Count];
            try
            {
                for (int i = 0; i < this.Count; i++)
                {
                    CustomProperty curprop = (CustomProperty)this[i];
                    if (curprop.Visible == true)
                    {
                        prop = curprop;
                        newProps[i] = new CustomPropertyDescriptor(ref prop, attributes);
                    }
                }
            }
            catch(Exception ex)
            {
                string s = string.Format("在读取 {0} 字段时出现错误，请修改元信息描述数据表。\r\n错误讯息为:{1}", prop.ID, ex.Message);
                System.Windows.Forms.MessageBox.Show(s);
            }

			return new PropertyDescriptorCollection(newProps);
		}

		public PropertyDescriptorCollection GetProperties()
		{			
			return TypeDescriptor.GetProperties(this, true);			
		}

		public object GetPropertyOwner(PropertyDescriptor pd) 
		{
			return this;
		}
		#endregion
	
	}

}
