using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data;
using System.Data.SqlClient;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Reflection;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace LogicalEditor
{
//         enum ELogicObjTypes
//         {
//             MAIN = 0,
//             NPC,
//             Doodad,
//             NPCRefreshPointSet,
//             NPCRefreshPoint,
//             NPCRefreshNPC,
//             DoodadRefreshPointSet,
//             DoodadRefreshPoint,
//             DoodadRefreshDoodad,
//             AIGroup,
//             AIGroupNPC,
//             WayPointSet,
//             WayPoint,
//             LogicalPoly,
//             LogicalPolyVertex,
//             Max
//         };
    public class LO_Map
    {
        public bool logicalChanged;
        public string name;
        public string filename;
        public LogicObj MAIN;
        public LogicObj[] NPCs;
        public LogicObj[] Doodads;
        public LogicObj[] NPCRefreshPointSets;
//      public LogicObj[] NPCRefreshPoint;
//      public LogicObj[] NPCRefreshNPC;
        public LogicObj[] DoodadRefreshPointSets;
//      public LogicObj[] DoodadRefreshPoint;
//      public LogicObj[] DoodadRefreshDoodad;
        public LogicObj[] AIGroups;
//      public LogicObj[] AIGroupNPC;
        public LogicObj[] WayPointSets;
//      public LogicObj[] WayPoint;
        public LogicObj[] LogicalPolys;
//      public LogicObj[] LogicalPolyVertex;
    };

    public class LogicObj : CollectionBase, ICustomTypeDescriptor, ICloneable
    {
        public bool changed; // 标识本身有否修改，用于保存的判断
        public string tyPe; // 那种类型的对象
        public int inDex; // 索引
        public int parentindex; // 父亲的索引
        public string[] fields; // 成员名
        public string[] values; // 成员值
        public bool[] fields_changed; // 标识某成员有否修改，用于黑体显示
        public LogicObj[] kids1; // 子对象最多有↓
        public LogicObj[] kids2; // 2个


        #region "IClonable implementation"
        public object Clone() // 明显不是Deep Clone, 连Shadow Clone也算不上。只是借这个名字而已。请继续路过。
        {
            LogicObj retobj = new LogicObj(this.tyPe);
            retobj.inDex = this.inDex;
            retobj.parentindex = this.parentindex;
            for (int i = 0; i < this.values.Length; i++)
                retobj.values[i] = this.values[i];
            return retobj;
        }
        #endregion

        #region "CollectionBase implementation"
        public void Add(object Value)
        {
            base.List.Add(Value);
        }
        #endregion

        #region "ICustomTypeDescriptor implementation"
        public String GetClassName()
        {
            return TypeDescriptor.GetClassName(this, true);
        }
        public AttributeCollection GetAttributes()
        {
            return TypeDescriptor.GetAttributes(this, true);
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
            // Create a collection object to hold property descriptors
			PropertyDescriptorCollection pds = new PropertyDescriptorCollection(null);

			// Iterate the list of employees
			for(int i = 0; i < this.List.Count; i++)
			{
				// Create a property descriptor for the employee item and add to the property descriptor collection
                PropertyDescriptor pd = new LogicalObjectCollectionPropertyDescriptor(this, i);
				pds.Add(pd);
			}
			// return the property descriptor collection
			return pds;
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


        public LogicObj(string typeName)
        {
            changed = false;
            tyPe = typeName;
            inDex = -1;
            parentindex = -1;

            switch (typeName)
            {
                case "MAIN":
                    fields = new string[]{
                        "NumNPC", "NumDoodad", "NumNpcRefreshSet", "NumNpcRefreshSetMaxID",
                        "NumDoodadRefreshSet", "NumDoodadRefreshSetMaxID", "NumAIGroup", "NumAIGroupMaxID",
                        "NumWapPointSet", "NumWapPointSetMaxID", "LogicalSceneID", "LogicalSceneXStart",
                        "LogicalSceneZStart", "NumLogicalPoly"
                    };
                    break;

                case "NPC":
                    fields = new string[] {
                        "szName", "szNickName", "szScriptName", "nTempleteID",
                        "nX", "nY", "nZ", "nKind",
                        "nFaceDirection", "nLevel", "dwScriptID", "dwRepresentID",
                        "vPosition", "vRotation", "WayPointSetID", "ReliveID",
                        "RandomID", "AISetID"
                    };
                    break;

                case "Doodad":
                    fields = new string[] {
                        "szName", "szNickName", "szScriptName", "nTempleteID",
                        "nX", "nY", "nZ", "nKind",
                        "nFaceDirection", "dwScriptID", "vPosition", "vRotation",
                        "ReliveID"
                    };
                    break;

                case "NPCRefreshPointSet":
                    fields = new string[] {
                        "szName", "dwSetID", "NumPoint", "NumNpc"
                    };
                    break;

                case "NPCRefreshPoint":
                    fields = new string[] {
                        "vPosition", "vRotation", "nX", "nY", "nZ", "nDirection"
                    };
                    break;

                case "NPCRefreshNPC":
                    fields = new string[] {
                        "Index"
                    };
                    break;

                case "DoodadRefreshPointSet":
                    fields = new string[] {
                        "szName", "dwSetID", "NumPoint", "NumDoodad"
                    };
                    break;

                case "DoodadRefreshPoint":
                    fields = new string[] {
                        "vPosition", "vRotation", "nX", "nY", "nZ", "nDirection"
                    };
                    break;

                case "DoodadRefreshDoodad":
                    fields = new string[] {
                        "Index"
                    };
                    break;

                case "AIGroup":
                    fields = new string[] {
                        "szName", "szFileName", "dwSetID", "nKing",
                        "nArrayIndex", "NumNPC", "WayPointSetID"
                    };
                    break;

                case "AIGroupNPC":
                    fields = new string[] {
                        "Index"
                    };
                    break;

                case "WayPointSet":
                    fields = new string[] {
                        "szName", "dwSetID", "NumPoint"
                    };
                    break;

                case "WayPoint":
                    fields = new string[] {
                        "vPosition", "vRotation", "nX", "nY",
                        "nZ", "nStayFrame", "nStayDirection", "nStayAnimation",
                        "bStayAniLoop", "szScriptName", "szInfo", "bIsRun"
                    };
                    break;

                case "LogicalPoly":
                    fields = new string[] {
                        "szFileName", "nLogicalSetID", "dwModifyState", "Color",
                        "NumVectex", "fHeight", "szPolyName"
                    };
                    break;

                case "LogicalPolyVertex":
                    fields = new string[] {
                        "vPosition"
                    };
                    break;

                default:
                    MessageBox.Show("wrong type!");
                    break;
            }
            // 新建value为数组。
            values = new string[fields.Length];
            // 新建有否修改的数组
            fields_changed = new bool[fields.Length];
        }

        public void Read()
        {
            string iniBlock = string.Empty;
            string iniFieldPrefix = string.Empty;

            switch (tyPe)
            {
                case "MAIN":
                    iniBlock = "MAIN";
                    break;

                case "NPC":
                    iniBlock = "NPC" + inDex.ToString();
                    break;
                case "Doodad":
                    iniBlock = "Doodad" + inDex.ToString();
                    break;

                case "NPCRefreshPointSet":
                    iniBlock = "NPCRefreshPointSet" + inDex.ToString();
                    break;
                case "NPCRefreshPoint":
                    iniBlock = "NPCRefreshPointSet" + parentindex.ToString();
                    iniFieldPrefix = "Point" + inDex.ToString();
                    break;
                case "NPCRefreshNPC":
                    iniBlock = "NPCRefreshPointSet" + parentindex.ToString();
                    iniFieldPrefix = "NPC" + inDex.ToString();
                    break;

                case "DoodadRefreshPointSet":
                    iniBlock = "DoodadRefreshPointSet" + inDex.ToString();
                    break;
                case "DoodadRefreshPoint":
                    iniBlock = "DoodadRefreshPointSet" + parentindex.ToString();
                    iniFieldPrefix = "Point" + inDex.ToString();
                    break;
                case "DoodadRefreshDoodad":
                    iniBlock = "DoodadRefreshPointSet" + parentindex.ToString();
                    iniFieldPrefix = "Doodad" + inDex.ToString();
                    break;

                case "AIGroup":
                    iniBlock = "AIGroup" + inDex.ToString();
                    break;
                case "AIGroupNPC":
                    iniBlock = "AIGroup" + parentindex.ToString();
                    iniFieldPrefix = "NPC" + inDex.ToString();
                    break;

                case "WayPointSet":
                    iniBlock = "WayPointSet" + inDex.ToString();
                    break;
                case "WayPoint":
                    iniBlock = "WayPointSet" + parentindex.ToString();
                    iniFieldPrefix = "Point" + inDex.ToString();
                    break;

                case "LogicalPoly":
                    iniBlock = "LogicalPoly" + inDex.ToString();
                    break;
                case "LogicalPolyVertex":
                    iniBlock = "LogicalPoly" + parentindex.ToString();
                    iniFieldPrefix = "Vertex" + inDex.ToString();
                    break;
            }

            StringBuilder sb = new StringBuilder(255);
            for (int i = 0; i < fields.Length; i++)
            {
                if (this.tyPe == "LogicalPolyVertex")
                    values[i] = Helper.GetIniString(iniBlock, iniFieldPrefix);
                else
                    values[i] = Helper.GetIniString(iniBlock, iniFieldPrefix + fields[i]);
            }
        }

        public void ForceInhandChange(bool changed)
        {
            this.changed = changed;
        }

        public void ForcePreviewChange(bool changed)
        {
            for (int i = 0; i < this.fields_changed.Length; i++)
                this.fields_changed[i] = changed;
        }

        public void ForcePreviewChange(bool changed, int index)
        {
            this.fields_changed[index] = changed;
        }
    };

    public class ObjDrawInfo
    {
        public PointF pfPos;    // 点于图的百分比位置，坐上为 (0.0, 0.0)
        public int nDir;      // 点的朝向
        public LogicObj lObj; // 对象ref

        public ObjDrawInfo(PointF pos, int dir, LogicObj obj)
        {
            this.pfPos = pos;
            this.nDir = dir;
            this.lObj = obj;
        }
    };

}

