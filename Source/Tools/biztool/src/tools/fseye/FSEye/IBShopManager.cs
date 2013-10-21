using System;
using System.Collections.Generic;
using System.Text;
using FSEye.DAL;
using System.Data;

namespace FSEye
{
    public class IBShopManager
    {
        public IBShopManager()
        {

        }

        public bool SaveScript(string script,out long scriptId)
        {
            IBlazeDatabase db = null;
            try
            {
                db = DbFactory.GetDatabase();
                IBlazeTable ibShopTable = db.GetTable(TableString.IbShopTableName);

                //插入数据到服务器表
                string[] fields = new string[] { 
					TableString.IbShopFieldTimeStamp,
					TableString.IbShopFieldScript,
				};
                object[] values = new object[] {
					DateTime.Now,
					script
				};

                db.BeginTrans();
                scriptId = ibShopTable.Add(fields, values);
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

        public IList<IBShopScript> GetScript(int scriptId)
        {
            IBlazeDatabase db = null;
            IList<IBShopScript> resultList = new List<IBShopScript>();
            try
            {
                db = DbFactory.GetDatabase();
                DataSet dataSet = new DataSet();

                IBlazeTable ibShopTable = db.GetTable(TableString.IbShopTableName);
                ibShopTable.Get(dataSet, FilterFactory.CreateEqualFilter(TableString.IbShopFieldId, scriptId));
                
                DataRowCollection rows = dataSet.Tables[TableString.IbShopTableName].Rows;
                foreach (DataRow row in rows)
                {
                    IBShopScript ibShopScript = new IBShopScript();
                    ibShopScript.ID = (int)row[TableString.IbShopFieldId];
                    ibShopScript.TimeStamp = (DateTime)row[TableString.IbShopFieldTimeStamp];
                    ibShopScript.Script = Encoding.Default.GetString(row[TableString.IbShopFieldScript] as byte[]);
                    resultList.Add(ibShopScript);
                }
                return resultList;
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

        public IList<IBShopScript> GetScript()
        {
            IBlazeDatabase db = null;
            IList<IBShopScript> resultList = new List<IBShopScript>();
            try
            {
                db = DbFactory.GetDatabase();
                DataSet dataSet = new DataSet();

                IBlazeTable ibShopTable = db.GetTable(TableString.IbShopTableName);
                ibShopTable.Get(dataSet);
                
                DataRowCollection rows = dataSet.Tables[TableString.IbShopTableName].Rows;
                foreach (DataRow row in rows)
                {
                    IBShopScript ibShopScript = new IBShopScript();
                    ibShopScript.ID = (int)row[TableString.IbShopFieldId];
                    ibShopScript.TimeStamp = (DateTime)row[TableString.IbShopFieldTimeStamp];
                    ibShopScript.Script = Encoding.Default.GetString(row[TableString.IbShopFieldScript] as byte[]);
                    resultList.Add(ibShopScript);
                }
                return resultList;
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

        public bool DeleteScript(int scriptId)
        {
            IBlazeDatabase db = null;
            try
            {
                db = DbFactory.GetDatabase();
                IBlazeTable ibShopTable = db.GetTable(TableString.IbShopTableName);

                db.BeginTrans();

                ibShopTable.Delete(FilterFactory.CreateEqualFilter(TableString.IbShopFieldId, scriptId));

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

    }

    public class IBShopScript
    {
        int _id;
        DateTime _timeStamp;
        string _script;

        public int ID
        {
            get { return _id; }
            set { _id = value; }
        }

        public DateTime TimeStamp
        {
            get { return _timeStamp; }
            set { _timeStamp = value; }
        }

        public string Script
        {
            get { return _script; }
            set { _script = value; }
        }

        public IBShopScript()
        {
        }

        public IBShopScript(int id, DateTime timeStamp, string script)
        {
            _id = id;
            _timeStamp = timeStamp;
            _script = script;
        }
    }
}
