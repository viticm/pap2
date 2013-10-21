using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace StandardNpcCreate
{
    class AnimationHelper
    {
        private static AnimationHelper animationHelper; // 动作文件辅助类对象
        private string rootPath; //客户端 根目录

        /// <summary>
        /// 私有构造函数
        /// </summary>
        private AnimationHelper()
        {

        }

        /// <summary>
        /// 客户端根目录
        /// </summary>
        public string RootPath
        {
            set
            {
                rootPath = value;
            }
        }

        /// <summary>
        /// 获取动作文件辅助类对象
        /// </summary>
        /// <returns>动作文件辅助类对象</returns>
        public static AnimationHelper GetAnimationHelper()
        {
            if (animationHelper == null)
            {
                animationHelper = new AnimationHelper();
            }

            return animationHelper;
        }

        /// <summary>
        /// 获取动作文件帧数
        /// </summary>
        /// <param name="animationFileName">动作文件名</param>
        /// <returns>动作文件帧数</returns>
        public int GetAniFrameCount(string animationFileName)
        {
            int aniFrameCount = 0;

            if (animationFileName.EndsWith(".tani"))
            {
                string fileName = Path.Combine(rootPath, animationFileName);
                FileInfo fi = new FileInfo(fileName);

                if (fi.Exists)
                {
                    FileStream fs = fi.OpenRead();                

                    // 跳过前面8个字节
                    for (int i = 0; i < 8; i++)
                    {
                        fs.ReadByte();                        
                    }

                    List<byte> byteList = new List<byte>();

                    while (true)
                    {
                        int charValue = fs.ReadByte();
                        byte b = (byte)charValue;
                        char c = Convert.ToChar(b);    

                        if (c == '\0')
                        {
                            break;
                        }
                        else
                        {
                            byteList.Add(b);
                        }
                    }

                    fs.Close();
                    
                    string aniFileName = Encoding.GetEncoding("gb2312").GetString(byteList.ToArray());
                    aniFrameCount = GetAnimationFramCount(aniFileName);
                }                
            }
            else if (animationFileName.EndsWith(".ani"))
            {
                aniFrameCount = GetAnimationFramCount(animationFileName);
            }

            return aniFrameCount;
        }

        /// <summary>
        /// 获取动作文件帧数
        /// </summary>
        /// <param name="animationFileName">动作文件名</param>
        /// <returns>动作文件帧数</returns>
        private int GetAnimationFramCount(string animationFileName)
        {
            int aniFrameCount = 0;

            string fileName = Path.Combine(rootPath, animationFileName);
            FileInfo fi = new FileInfo(fileName);

            if (fi.Exists)
            {
                FileStream fs = fi.OpenRead();

                // 跳过前面12个字节
                for (int i = 0; i < 12; i++)
                {
                    fs.ReadByte();
                }

                StringBuilder aniFileName = new StringBuilder();
                byte[] buffer = new byte[4];

                for (int i = 0; i < 4; i++)
                {
                    buffer[i] = (byte)fs.ReadByte();                    
                }

                int animationType = (int)BitConverter.ToUInt32(buffer, 0);
                int frameCount = 0;
                float frameLength = 0;

                switch (animationType)
                {
                    case 1: // ANIMATION_VERTICES
                        {
                            // 跳过38个字节
                            for (int i = 0; i < 38; i++)
                            {
                                fs.ReadByte();
                            }

                            buffer = new byte[4];

                            for (int i = 0; i < 4; i++)
                            {
                                buffer[i] = (byte)fs.ReadByte();
                            }
                            frameCount = (int)BitConverter.ToUInt32(buffer, 0);

                            buffer = new byte[4];

                            for (int i = 0; i < 4; i++)
                            {
                                buffer[i] = (byte)fs.ReadByte();
                            }
                            frameLength = (float)BitConverter.ToSingle(buffer, 0);

                            break;
                        }
                    case 2: // ANIMATION_VERTICES_RELATIVE
                        {
                            for (int i = 0; i < 38; i++)
                            {
                                fs.ReadByte();
                            }

                            buffer = new byte[4];

                            for (int i = 0; i < 4; i++)
                            {
                                buffer[i] = (byte)fs.ReadByte();
                            }
                            frameCount = (int)BitConverter.ToUInt32(buffer, 0);

                            buffer = new byte[4];

                            for (int i = 0; i < 4; i++)
                            {
                                buffer[i] = (byte)fs.ReadByte();
                            }
                            frameLength = (float)BitConverter.ToSingle(buffer, 0);

                            break;
                        }
                    case 10: // ANIMATION_BONE
                        {
                            for (int i = 0; i < 34; i++)
                            {
                                fs.ReadByte();
                            }

                            buffer = new byte[4];

                            for (int i = 0; i < 4; i++)
                            {
                                buffer[i] = (byte)fs.ReadByte();
                            }
                            frameCount = (int)BitConverter.ToUInt32(buffer, 0);

                            buffer = new byte[4];

                            for (int i = 0; i < 4; i++)
                            {
                                buffer[i] = (byte)fs.ReadByte();
                            }
                            frameLength = (float)BitConverter.ToSingle(buffer, 0);

                            break;
                        }
                    case 11: // ANIMATION_BONE_RELATIVE
                        {
                            for (int i = 0; i < 34; i++)
                            {
                                fs.ReadByte();
                            }

                            buffer = new byte[4];

                            for (int i = 0; i < 4; i++)
                            {
                                buffer[i] = (byte)fs.ReadByte();
                            }
                            frameCount = (int)BitConverter.ToUInt32(buffer, 0);

                            buffer = new byte[4];

                            for (int i = 0; i < 4; i++)
                            {
                                buffer[i] = (byte)fs.ReadByte();
                            }
                            frameLength = (float)BitConverter.ToSingle(buffer, 0);

                            break;
                        }
                    case 13: // ANIMATION_BONE_RTS
                        {
                            for (int i = 0; i < 34; i++)
                            {
                                fs.ReadByte();
                            }

                            buffer = new byte[4];

                            for (int i = 0; i < 4; i++)
                            {
                                buffer[i] = (byte)fs.ReadByte();
                            }
                            frameCount = (int)BitConverter.ToUInt32(buffer, 0);

                            buffer = new byte[4];

                            for (int i = 0; i < 4; i++)
                            {
                                buffer[i] = (byte)fs.ReadByte();
                            }
                            frameLength = (float)BitConverter.ToSingle(buffer, 0);

                            break;
                        }                        
                }

                if (frameCount != 0 && frameLength != 0) // 数值有效
                {
                    aniFrameCount = (int)((frameCount - 1) * frameLength * 16f / 1000) + 1;
                }

                fs.Close();                
            }

            return aniFrameCount;
        }
    }
}
