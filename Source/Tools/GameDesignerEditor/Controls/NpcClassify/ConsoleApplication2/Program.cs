using System;
using System.Collections.Generic;
using System.Text;
using System.Data.SqlClient;
using System.Windows.Forms;
using System.Data;

namespace NpcClassify
{
    class Program
    {
        Form1 frm = null; // 显示的窗口
        private string lastnpcid = "0"; // 上一个选择的npc的id
        private string lastdoodadid = "0"; // 上一个选择的小物件的id
        private string lastitemid = "0"; // 上一个选择的物品的id

        /// <summary>
        /// 构造函数
        /// </summary>
        public Program()
        {
        }

        /// <summary>
        /// 显示分类界面并返回用户选择值
        /// </summary>
        /// <param name="Conn">sql连接</param>
        /// <param name="oldValue">旧值</param>
        /// <param name="mode">模式序号</param>
        /// <returns>选择id</returns>
        public string Show(SqlConnection Conn, string oldValue, int mode)
        {
            try
            {
                if (Conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    Conn.Open();
                }

                if(oldValue == string.Empty) // 空值转换
                {
                    oldValue = "0";
                }

                switch (mode) // 根据mode值选择不同的处理模式
                {
                    case 1: // 处理Npc分类
                        {
                            if (oldValue == "0")
                            {
                                frm = new Form1(Conn, "NpcTemplate", "MapName", "ForceID", 1, lastnpcid);
                            }
                            else
                            {
                                frm = new Form1(Conn, "NpcTemplate", "MapName", "ForceID", 1, oldValue);
                            }
                            frm.Text = "NPC分类查找";
                            break;
                        }
                    case 2: // 处理小物件分类
                        {
                            if (oldValue == "0")
                            {
                                frm = new Form1(Conn, "tbl_doodad", "MapName", "Kind", 2, lastdoodadid);
                            }
                            else
                            {
                                frm = new Form1(Conn, "tbl_doodad", "MapName", "Kind", 2, oldValue);
                            }
                            frm.Text = "Doodad分类查找";
                            break;
                        }
                }
                frm.StartPosition = FormStartPosition.CenterScreen; // 将窗口设置为屏幕正中位置
                DialogResult result = frm.ShowDialog();
                if (result == DialogResult.OK) // 点击确定
                {
                    switch (mode)
                    {
                        case 1:
                            {
                                lastnpcid = frm.Result;
                                break;
                            }
                        case 2:
                            {
                                lastdoodadid = frm.Result;
                                break;
                            }
                    }
                    return frm.Result; // 返回查询到的值
                }
                else // 点击取消
                {
                    return oldValue; // 返回原值
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception occured: " + ex.ToString());
                return oldValue.ToString();
            }
            finally
            {
                if (Conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    Conn.Close();
                }
            }
        }

        /// <summary>
        /// 显示分类界面并返回用户选择值
        /// </summary>
        /// <param name="Conn">sql连接</param>
        /// <param name="mapIndex">地图序号</param>
        /// <param name="oldValue">旧值</param>
        /// <param name="mode">模式序号</param>
        /// <returns>选择id</returns>
        public string Show(SqlConnection Conn, string mapIndex, string oldValue, int mode)
        {
            try
            {
                if (Conn.State == ConnectionState.Closed) // 打开sql连接
                {
                    Conn.Open();
                }

                if (oldValue == string.Empty) // 空值转换
                {
                    oldValue = "0";
                }

                if (mapIndex == string.Empty) // 空值转换
                {
                    mapIndex = "0";
                }

                if (oldValue == "0")
                {
                    frm = new Form1(Conn, "Genre", "_CATEGORY", mode, mapIndex, lastitemid); // 显示的窗口
                }
                else
                {
                    frm = new Form1(Conn, "Genre", "_CATEGORY", mode, mapIndex, oldValue); // 显示的窗口 
                }
                frm.Text = "Item分类查找";
                frm.StartPosition = FormStartPosition.CenterScreen; // 将窗口设置为屏幕正中位置
                DialogResult result = frm.ShowDialog();
                if (result == DialogResult.OK) // 点击确定
                {
                    lastitemid = frm.Result;
                    return frm.Result; // 返回查询到的值
                }
                else // 点击取消
                {
                    return oldValue; // 返回原值
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Exception occured: " + ex.ToString()); // 显示异常信息
                return oldValue;
            }
            finally
            {
                if (Conn.State == ConnectionState.Open) // 关闭sql连接
                {
                    Conn.Close();
                }
            }
        }

        /// <summary>
        /// 显示对话窗口
        /// </summary>
        /// <param name="message">对话窗口</param>
        /// <param name="title">标题</param>
        public void Show(string message, string title)
        {
            MessageBox.Show(message, title);
        }

        /// <summary>
        /// 返回地图类型值
        /// </summary>
        /// <returns>地图类型</returns>
        public string GetMapType()
        {
            return frm.MapIndex;
        }

        /// <summary>
        /// 返回地图名称
        /// </summary>
        /// <returns>地图名称</returns>
        public string GetMapName()
        {
            return frm.MapName;
        }

        /// <summary>
        /// 获取小物件类型
        /// </summary>
        /// <returns>小物件类型序号</returns>
        public string GetDoodadKind()
        {
            return frm.DoodadKind;
        }
    }
}
