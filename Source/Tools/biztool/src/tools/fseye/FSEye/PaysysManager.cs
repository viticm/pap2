using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using FSEye.DAL;

namespace FSEye
{
    public class PaysysManager
    {
        public PaysysManager()
        { }

        /// <summary>
        /// 保存帐号密码
        /// </summary>
        public bool SaveAccountPassword(string accountName, string accountPassword)
        {
            IBlazeDatabase db = null;
            try
            {
                db = DbFactory.GetDatabase();
                IBlazeTable accountTable = db.GetTable(TableString.AccountInfoTableName);

                //插入数据到服务器表
                string[] fields = new string[] { 
					TableString.AccountInfoFieldName,
					TableString.AccountInfoFieldPassword,					
				};
                object[] values = new object[] {
					accountName,
					accountPassword
				};

                db.BeginTrans();
                if (GetSavedPassword(accountName) != null)
                {
                    //Update
                    accountTable.Set(TableString.AccountInfoFieldPassword, accountPassword,
                        FilterFactory.CreateEqualFilter(TableString.AccountInfoFieldName, accountName));
                }
                else
                {
                    //Insert
                    accountTable.Add(fields, values);
                }
                db.CommitTrans();

                return true;
            }
            catch (Exception ex)
            {
                if (db != null)
                    db.RollbackTrans();
                throw ex;                
            }
            finally
            {
                if (db != null)
                    db.Close();                
            }
        }

        /// <summary>
        /// 得到已保存的帐号密码
        /// </summary>
        public string GetSavedPassword(string accountName)
        {
            IBlazeDatabase db = null;
            try
            {
                db = DbFactory.GetDatabase();
                DataSet dataSet = new DataSet();

                IBlazeTable accountTable = db.GetTable(TableString.AccountInfoTableName);
                accountTable.Get(dataSet, FilterFactory.CreateEqualFilter(TableString.AccountInfoFieldName, accountName));
                DataRowCollection rows = dataSet.Tables[TableString.AccountInfoTableName].Rows;
                if (rows.Count != 0)
                    return rows[0][TableString.AccountInfoFieldPassword] as string;
                return null;
            }
            catch (Exception ex)
            {
                throw ex;
            }
            finally
            {
                if (db != null)
                    db.Close();
            }
        }

        public void Dispose()
        { }
    }
}
