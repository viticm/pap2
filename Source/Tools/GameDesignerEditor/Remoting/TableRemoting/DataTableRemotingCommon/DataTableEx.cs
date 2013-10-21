using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Runtime.Serialization;

namespace DataTableRemotingCommon
{
    [Serializable]
    public class DataTableEx : DataTable
    {

        private List<object[]> mDeleteRowList;
        private bool mCanWrite;

        public List<string> mKeyColumns;

        public List<object[]> DeleteRowList { get { return mDeleteRowList; } }
        public bool CanWrite { get { return mCanWrite; } }

        public void SetCanWrite(bool enable)
        {
            mCanWrite = enable;
        }

        public DataTableEx()
        {
            mCanWrite = false;
            mDeleteRowList = new List<object[]>();
            mKeyColumns = new List<string>();
            foreach (DataColumn column in PrimaryKey)
            {
                mKeyColumns.Add(column.ColumnName);
            }
        }

        #region DataTableEx Events

        protected override void OnColumnChanging(DataColumnChangeEventArgs e)
        {
            if (mCanWrite)
            {
                (e.Row as DataRowEx).OnRowChanged();
            }
            base.OnColumnChanging(e);
        }


        protected override void OnRowDeleting(DataRowChangeEventArgs e)
        {
            if (mCanWrite && e.Action == DataRowAction.Delete)
            {
                if (!(e.Row as DataRowEx).IsNewRow)
                {
                    object[] keyValues = (e.Row as DataRowEx).KeyValues;
                    if (keyValues == null)
                    {
                        keyValues = (e.Row as DataRowEx).GetKeyValues();
                        (e.Row as DataRowEx).OnRowChanged();
                    }
                    mDeleteRowList.Add(keyValues);
                }
            }
            base.OnRowDeleting(e);
        }

        protected override void OnRowChanged(DataRowChangeEventArgs e)
        {
            if (mCanWrite && e.Action == DataRowAction.Add)
            {
                (e.Row as DataRowEx).OnAddRow();
            }
            base.OnRowChanged(e);
        }


        #endregion

        #region DataTableEx Setting

        protected DataTableEx(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
            mCanWrite = true;
            mDeleteRowList = new List<object[]>();
            mKeyColumns = new List<string>();
            foreach (DataColumn column in PrimaryKey)
            {
                mKeyColumns.Add(column.ColumnName);
            }
        }

        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            base.GetObjectData(info, context);

        }

        public override DataTable Clone()
        {
            DataTableEx tableEx = base.Clone() as DataTableEx;
            return tableEx;
        }

        protected override DataTable CreateInstance()
        {
            return new DataTableEx();
        }

        protected override DataRow NewRowFromBuilder(DataRowBuilder builder)
        {
            return new DataRowEx(builder);
        }

        protected override Type GetRowType()
        {
            return typeof(DataRowEx);
        }


        #endregion
    }


    public class DataRowEx : DataRow
    {
        private object[] mKeyValues;
        private bool mIsNewRow;

        public string Key;

        /// <summary>
        /// 构造函数
        /// </summary>
        /// <param name="builder">DataRowBuilder</param>
        internal DataRowEx(DataRowBuilder builder)
            : base(builder)
        {
            mIsNewRow = false;
            mKeyValues = null;
        }

        /// <summary>
        /// 取键值数组
        /// </summary>
        /// <returns></returns>
        public object[] GetKeyValues()
        {
            int len = (Table as DataTableEx).mKeyColumns.Count;

            object[] result = new object[len];
            int index = 0;
            foreach (string column in (Table as DataTableEx).mKeyColumns)
            {
                result[index] = this[column];
                index++;
            }
            return result;
        }

        /// <summary>
        /// 取所有例值的数组
        /// </summary>
        /// <returns></returns>
        public object[] GetValues()
        {
            object[] result = new object[this.Table.Columns.Count];
            for (int index = 0; index < result.Length; ++index)
            {
                result[index] = this[index];
            }
            return result;
        }

        /// <summary>
        /// 行被修改时执行
        /// </summary>
        public void OnRowChanged()
        {
            if (mKeyValues == null)
            {
                mKeyValues = GetKeyValues();
            }
        }

        /// <summary>
        /// 新行
        /// </summary>
        public void OnAddRow()
        {
            mIsNewRow = true;
            OnRowChanged();
        }

        /// <summary>
        /// 更新行
        /// </summary>
        public void Update()
        {
            mIsNewRow = false;
            mKeyValues = null;
        }


        public object[] KeyValues { get { return mKeyValues; } }
        public bool IsNewRow { get { return mIsNewRow; } }

    }
}
