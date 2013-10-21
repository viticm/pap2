using System;
using System.Collections.Generic;
using System.Text;

namespace CustomLoadTree
{
    class TaskData
    {
        private int[] timeArray = new int[71]; // 等级和杀怪时间对应数组
        private int[] expArray = new int[71]; // 等级和经验对应数组
        private int[] moneyArray = new int[70]; // 等级和金钱对应数组
        private int[] normalTimeArray1 = new int[70]; // 等级和标准时间对应数组(对话任务)
        private int[] normalTimeArray2 = new int[70]; // 等级和标准时间对应数组(送信任务)
        private int[] normalTimeArray3 = new int[70]; // 等级和标准时间对应数组(采集和脚本任务)
        private int[] normalTimeArray4 = new int[70]; // 等级和标准时间对应数组(杀怪和收集任务)
        private int[] normalTimeArray5 = new int[70]; // 等级和标准时间对应数组(高级任务)
        private int[] normalTimeArray6 = new int[70]; // 等级和标准时间对应数组(精英任务)
        private int[] totalMoneyArray = new int[70]; // 等级和总金钱对应数组

        public TaskData()
        {
            FillTimeArray(); // 填充等级和刷怪时间对应数组
            FillExpArray(); // 填充等级经验对应数组
            FillMoneyArray(); // 填充等级金钱对应数组
            FillNormalTimeArray(); // 填充等级和标准时间对应数组
            FillTotalExpArray(); // 填充等级总经验数组
        }

        /// <summary>
        /// 填充等级和刷怪时间对应数组
        /// </summary>
        private void FillTimeArray()
        {
            timeArray[0] = 0;
            timeArray[1] = 18;
            timeArray[2] = 19;
            timeArray[3] = 19;
            timeArray[4] = 20;
            timeArray[5] = 20;
            timeArray[6] = 21;
            timeArray[7] = 22;
            timeArray[8] = 22;
            timeArray[9] = 23;
            timeArray[10] = 23;
            timeArray[11] = 24;
            timeArray[12] = 25;
            timeArray[13] = 25;
            timeArray[14] = 26;
            timeArray[15] = 27;
            timeArray[16] = 27;
            timeArray[17] = 28;
            timeArray[18] = 28;
            timeArray[19] = 29;
            timeArray[20] = 30;
            timeArray[21] = 30;
            timeArray[22] = 31;
            timeArray[23] = 31;
            timeArray[24] = 32;
            timeArray[25] = 33;
            timeArray[26] = 33;
            timeArray[27] = 34;
            timeArray[28] = 34;
            timeArray[29] = 35;
            timeArray[30] = 36;
            timeArray[31] = 36;
            timeArray[32] = 37;
            timeArray[33] = 37;
            timeArray[34] = 38;
            timeArray[35] = 39;
            timeArray[36] = 39;
            timeArray[37] = 40;
            timeArray[38] = 41;
            timeArray[39] = 41;
            timeArray[40] = 42;
            timeArray[41] = 42;
            timeArray[42] = 43;
            timeArray[43] = 44;
            timeArray[44] = 44;
            timeArray[45] = 45;
            timeArray[46] = 45;
            timeArray[47] = 46;
            timeArray[48] = 47;
            timeArray[49] = 47;
            timeArray[50] = 48;
            timeArray[51] = 48;
            timeArray[52] = 49;
            timeArray[53] = 50;
            timeArray[54] = 50;
            timeArray[55] = 50;
            timeArray[56] = 50;
            timeArray[57] = 50;
            timeArray[58] = 50;
            timeArray[59] = 50;
            timeArray[60] = 50;
            timeArray[61] = 50;
            timeArray[62] = 50;
            timeArray[63] = 50;
            timeArray[64] = 50;
            timeArray[65] = 50;
            timeArray[66] = 50;
            timeArray[67] = 50;
            timeArray[68] = 50;
            timeArray[69] = 50;
            timeArray[70] = 50;
        }

        /// <summary>
        /// 填充等级经验对应数组
        /// </summary>
        private void FillExpArray()
        {
            expArray[0] = 0;
            expArray[1] = 106;
            expArray[2] = 116;
            expArray[3] = 118;
            expArray[4] = 126;
            expArray[5] = 130;
            expArray[6] = 138;
            expArray[7] = 143;
            expArray[8] = 146;
            expArray[9] = 152;
            expArray[10] = 157;
            expArray[11] = 160;
            expArray[12] = 164;
            expArray[13] = 168;
            expArray[14] = 172;
            expArray[15] = 174;
            expArray[16] = 182;
            expArray[17] = 188;
            expArray[18] = 195;
            expArray[19] = 200;
            expArray[20] = 206;
            expArray[21] = 212;
            expArray[22] = 217;
            expArray[23] = 222;
            expArray[24] = 227;
            expArray[25] = 232;
            expArray[26] = 237;
            expArray[27] = 241;
            expArray[28] = 245;
            expArray[29] = 249;
            expArray[30] = 253;
            expArray[31] = 257;
            expArray[32] = 261;
            expArray[33] = 265;
            expArray[34] = 268;
            expArray[35] = 272;
            expArray[36] = 275;
            expArray[37] = 278;
            expArray[38] = 281;
            expArray[39] = 284;
            expArray[40] = 287;
            expArray[41] = 290;
            expArray[42] = 293;
            expArray[43] = 296;
            expArray[44] = 298;
            expArray[45] = 301;
            expArray[46] = 304;
            expArray[47] = 306;
            expArray[48] = 308;
            expArray[49] = 311;
            expArray[50] = 313;
            expArray[51] = 315;
            expArray[52] = 317;
            expArray[53] = 317;
            expArray[54] = 323;
            expArray[55] = 329;
            expArray[56] = 335;
            expArray[57] = 341;
            expArray[58] = 347;
            expArray[59] = 352;
            expArray[60] = 358;
            expArray[61] = 364;
            expArray[62] = 370;
            expArray[63] = 376;
            expArray[64] = 382;
            expArray[65] = 388;
            expArray[66] = 394;
            expArray[67] = 400;
            expArray[68] = 406;
            expArray[69] = 412;
            expArray[70] = 418;
        }

        /// <summary>
        /// 填充等级总经验数组
        /// </summary>
        private void FillTotalExpArray()
        {
            totalMoneyArray[0] = 0;
            totalMoneyArray[1] = 17;
            totalMoneyArray[2] = 56;
            totalMoneyArray[3] = 104;
            totalMoneyArray[4] = 185;
            totalMoneyArray[5] = 301;
            totalMoneyArray[6] = 464;
            totalMoneyArray[7] = 692;
            totalMoneyArray[8] = 1007;
            totalMoneyArray[9] = 1440;
            totalMoneyArray[10] = 2032;
            totalMoneyArray[11] = 2729;
            totalMoneyArray[12] = 3634;
            totalMoneyArray[13] = 4806;
            totalMoneyArray[14] = 6320;
            totalMoneyArray[15] = 8261;
            totalMoneyArray[16] = 9613;
            totalMoneyArray[17] = 10994;
            totalMoneyArray[18] = 12533;
            totalMoneyArray[19] = 14250;
            totalMoneyArray[20] = 16327;
            totalMoneyArray[21] = 18470;
            totalMoneyArray[22] = 21046;
            totalMoneyArray[23] = 23714;
            totalMoneyArray[24] = 26903;
            totalMoneyArray[25] = 28779;
            totalMoneyArray[26] = 31313;
            totalMoneyArray[27] = 34080;
            totalMoneyArray[28] = 38610;
            totalMoneyArray[29] = 43335;
            totalMoneyArray[30] = 48153;
            totalMoneyArray[31] = 51378;
            totalMoneyArray[32] = 54773;
            totalMoneyArray[33] = 58346;
            totalMoneyArray[34] = 61711;
            totalMoneyArray[35] = 65657;
            totalMoneyArray[36] = 69391;
            totalMoneyArray[37] = 73752;
            totalMoneyArray[38] = 77895;
            totalMoneyArray[39] = 82252;
            totalMoneyArray[40] = 87311;
            totalMoneyArray[41] = 91893;
            totalMoneyArray[42] = 96707;
            totalMoneyArray[43] = 101764;
            totalMoneyArray[44] = 107615;
            totalMoneyArray[45] = 113213;
            totalMoneyArray[46] = 119094;
            totalMoneyArray[47] = 125273;
            totalMoneyArray[48] = 131763;
            totalMoneyArray[49] = 138581;
            totalMoneyArray[50] = 145744;
            totalMoneyArray[51] = 155764;
            totalMoneyArray[52] = 165704;
            totalMoneyArray[53] = 177002;
            totalMoneyArray[54] = 189016;
            totalMoneyArray[55] = 200958;
            totalMoneyArray[56] = 214493;
            totalMoneyArray[57] = 227964;
            totalMoneyArray[58] = 240892;
            totalMoneyArray[59] = 253498;
            totalMoneyArray[60] = 266727;
            totalMoneyArray[61] = 280610;
            totalMoneyArray[62] = 296289;
            totalMoneyArray[63] = 311616;
            totalMoneyArray[64] = 327695;
            totalMoneyArray[65] = 343309;
            totalMoneyArray[66] = 360950;
            totalMoneyArray[67] = 379454;
            totalMoneyArray[68] = 398860;
            totalMoneyArray[69] = 417746;
        }

        /// <summary>
        /// 填充等级金钱对应数组
        /// </summary>
        private void FillMoneyArray()
        {
            moneyArray[0] = 0;
            moneyArray[1] = 6;
            moneyArray[2] = 7;
            moneyArray[3] = 8;
            moneyArray[4] = 11;
            moneyArray[5] = 14;
            moneyArray[6] = 17;
            moneyArray[7] = 22;
            moneyArray[8] = 27;
            moneyArray[9] = 34;
            moneyArray[10] = 43;
            moneyArray[11] = 53;
            moneyArray[12] = 64;
            moneyArray[13] = 78;
            moneyArray[14] = 94;
            moneyArray[15] = 114;
            moneyArray[16] = 124;
            moneyArray[17] = 133;
            moneyArray[18] = 142;
            moneyArray[19] = 153;
            moneyArray[20] = 165;
            moneyArray[21] = 177;
            moneyArray[22] = 192;
            moneyArray[23] = 206;
            moneyArray[24] = 223;
            moneyArray[25] = 242;
            moneyArray[26] = 263;
            moneyArray[27] = 275;
            moneyArray[28] = 300;
            moneyArray[29] = 325;
            moneyArray[30] = 349;
            moneyArray[31] = 361;
            moneyArray[32] = 372;
            moneyArray[33] = 385;
            moneyArray[34] = 395;
            moneyArray[35] = 409;
            moneyArray[36] = 420;
            moneyArray[37] = 435;
            moneyArray[38] = 448;
            moneyArray[39] = 461;
            moneyArray[40] = 478;
            moneyArray[41] = 491;
            moneyArray[42] = 506;
            moneyArray[43] = 521;
            moneyArray[44] = 539;
            moneyArray[45] = 556;
            moneyArray[46] = 573;
            moneyArray[47] = 591;
            moneyArray[48] = 610;
            moneyArray[49] = 630;
            moneyArray[50] = 651;
            moneyArray[51] = 685;
            moneyArray[52] = 716;
            moneyArray[53] = 753;
            moneyArray[54] = 792;
            moneyArray[55] = 829;
            moneyArray[56] = 873;
            moneyArray[57] = 915;
            moneyArray[58] = 953;
            moneyArray[59] = 990;
            moneyArray[60] = 1029;
            moneyArray[61] = 1069;
            moneyArray[62] = 1116;
            moneyArray[63] = 1160;
            moneyArray[64] = 1207;
            moneyArray[65] = 1251;
            moneyArray[66] = 1302;
            moneyArray[67] = 1355;
            moneyArray[68] = 1411;
            moneyArray[69] = 1465;
        }

        /// <summary>
        /// 填充等级和标准时间对应数组
        /// </summary>
        private void FillNormalTimeArray()
        {
            // 填充等级和标准时间对应数组(对话任务)
            normalTimeArray1[0] = 0;
            for(int i = 1; i <= 14; i++)
            {
                normalTimeArray1[i] = 120;
            }
            
            for(int i = 15; i <= 24; i++)
            {
                normalTimeArray1[i] = 180;
            }

            for(int i = 25; i <= 29; i++)
            {
                normalTimeArray1[i] = 240;
            }

            for(int i = 30; i <= 69; i++)
            {
                normalTimeArray1[i] = 300;
            }

            // 填充等级和标准时间对应数组(送信任务)
            for(int i = 0; i < normalTimeArray1.Length; i++)
            {
                normalTimeArray2[i] = normalTimeArray1[i];
            }

            // 填充等级和标准时间对应数组(采集和脚本任务)
            normalTimeArray3[0] = 0;
            for(int i = 1; i <= 4; i++)
            {
                normalTimeArray3[i] = 180;
            }

            for(int i = 5; i <= 9; i++)
            {
                normalTimeArray3[i] = 360;
            }

            for(int i = 10; i <= 14; i++)
            {
                normalTimeArray3[i] = 540;
            }

            for(int i = 15; i <= 59; i++)
            {
                normalTimeArray3[i] = 720;
            }

            for(int i = 60; i <= 69; i++)
            {
                normalTimeArray3[i] = 900;
            }

            // 填充等级和标准时间对应数组(杀怪和收集任务)
            for(int i = 0; i < normalTimeArray3.Length; i++)
            {
                normalTimeArray4[i] = (int)(normalTimeArray3[i] * 5 / 3);
            }

            // 填充等级和标准时间对应数组(高级任务) 和 填充等级和标准时间对应数组(精英任务)
            for(int i = 0; i < normalTimeArray4.Length; i++)
            {
                normalTimeArray5[i] = normalTimeArray4[i] + 300;
                normalTimeArray6[i] = normalTimeArray4[i] + 300;
            }
        }

        /// <summary>
        /// 等级和杀怪时间对应数组属性
        /// </summary>
        public int[] TimeArray 
        {
            get
            {
                return timeArray;
            }
        }

        /// <summary>
        /// 玩家等级和经验键对应数组属性
        /// </summary>
        public int[] ExpArray
        {
            get
            {
                return expArray;
            }
        }

        /// <summary>
        /// 等级和金钱对应数组属性
        /// </summary>
        public int[] MoneyArray 
        {
            get
            {
                return moneyArray;
            }
        }

        /// <summary>
        /// 等级和标准时间对应数组(对话任务)属性
        /// </summary>
        public int[] NormalTimeArray1
        {
            get
            {
                return normalTimeArray1;
            }
        }

        /// <summary>
        /// 等级和标准时间对应数组(送信任务)属性
        /// </summary>
        public int[] NormalTimeArray2
        {
            get
            {
                return normalTimeArray2;
            }
        }

        /// <summary>
        /// 等级和标准时间对应数组(采集和脚本任务)属性
        /// </summary>
        public int[] NormalTimeArray3
        {
            get
            {
                return normalTimeArray3;
            }
        }

        /// <summary>
        /// 等级和标准时间对应数组(杀怪和收集任务)属性
        /// </summary>
        public int[] NormalTimeArray4
        {
            get
            {
                return normalTimeArray4;
            }
        }

        /// <summary>
        /// 等级和标准时间对应数组(高级任务)属性
        /// </summary>
        public int[] NormalTimeArray5
        {
            get
            {
                return normalTimeArray5;
            }
        }

        /// <summary>
        /// 等级和标准时间对应数组(精英任务)属性
        /// </summary>
        public int[] NormalTimeArray6
        {
            get
            {
                return normalTimeArray6;
            }
        }

        /// <summary>
        /// 等级和总金钱对应数组属性
        /// </summary>
        public int[] TotalMoneyArray
        {
            get
            {
                return totalMoneyArray;
            }
        }
    }
}
