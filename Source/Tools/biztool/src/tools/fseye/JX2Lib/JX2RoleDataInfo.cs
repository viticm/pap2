using System;
using System.Collections.Generic;
using System.Text;
using JX2Lib;
namespace JX2Lib
{
    public class JX2RoleDataInfo
    {
        public string cRoleID;
        public string cAccountName;
        public string cRoleName;
        public JX2RoleSex iSex;
        public int iSeries;
        public int iLevel;
        public int iCamp;
        public int iPkValue;
        public int iExp;
        public int iLife;
        public int iMana;
        public int iStamina;
        public int iGatherPoint;
        public int iMakePoint;
        public int iRemainPotential;
        public int iStrength;
        public int iDexterity;
        public int iVitality;
        public int iEnergy;
        public int iSkillPoint;
        public DateTime dCreateTime;
        public DateTime dLastLoginTime;
        public int iGameTime;
        public int cLastLoginIP;
        public int iFaction;
        public int iFactionRoute;
        public JxRoleType RoleType;
        public Int64 iMoney;
        public int iTongID;

        public JX2RoleDataInfo()
        {
            cAccountName = "";
            cRoleName = "";
            iSex = 0;
            iLevel=0;
            iFaction=0;
            iFactionRoute=0;
            RoleType = 0;
            iMoney=0;
            dCreateTime=DateTime.Now;
            dLastLoginTime=DateTime.Now;
            iGameTime = 0;
        }

    }
}
