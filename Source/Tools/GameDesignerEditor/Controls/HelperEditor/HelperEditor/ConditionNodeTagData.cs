using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HelperEditor
{
    [Serializable]
    public class ConditionNodeTagData
    {
        private string szConditionID;
        private string szShowName;
        private string szCompareSourceCode;
        private DorpDownListItem compareOperator;
        private DorpDownListItem compareTarget;
        private DorpDownListItem compareCondition;


        /// <summary>
        /// 通过条件 ID 创建一个 Tag 数据对象。
        /// </summary>
        /// <param name="szID">ConditionID。</param>
        public ConditionNodeTagData(string szID, string szName, string szCode, DorpDownListItem operatorItem, DorpDownListItem targetItem, DorpDownListItem conditionItem)
        {
            szConditionID = szID;
            szShowName = szName;
            szCompareSourceCode = szCode;
            compareOperator = operatorItem;
            compareTarget = targetItem;
            compareCondition = conditionItem;
        }

        public string ConditionID
        {
            set
            {
                szConditionID = value;
            }
            get
            {
                return szConditionID;
            }
        }

        public DorpDownListItem CompareCondition
        {
            set
            {
                compareCondition = value;
            }
            get
            {
                return compareCondition;
            }
        }

        public string ShownString
        {
            set
            {
                szShowName = value;
            }
            get
            {
                return szShowName;
            }
        }

        public string CompareCode
        {
            set
            {
                szCompareSourceCode = value;
            }
            get
            {
                return szCompareSourceCode;
            }
        }

        public DorpDownListItem CompareOperator
        {
            set
            {
                compareOperator = value;
            }
            get
            {
                return compareOperator;
            }
        }

        public DorpDownListItem CompareTarget
        {
            set
            {
                compareTarget = value;
            }
            get
            {
                return compareTarget;
            }
        }
    }
}
