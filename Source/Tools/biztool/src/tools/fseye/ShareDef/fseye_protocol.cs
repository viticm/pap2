using System;
using System.IO;
using System.Text;
using System.Data;
using System.Collections;
using System.Collections.Generic;

public interface ISerial
{
    void Load(ref BinaryReader reader);
    void Save(ref BinaryWriter writer);
};

public interface IProtocol
{
    UInt16 ProtocolId
    {
        get;
    }
    byte[] ToBytes();
    void FromBytes(byte[] buffer);
    ISubProtocol GetSubProtocol();
};

public interface ISubProtocol : IProtocol
{
    UInt16 SubProtocolID
    {
        get;
    }
};

public abstract class KSerial : ISerial
{
    public abstract void Load(ref BinaryReader reader);
    public abstract void Save(ref BinaryWriter writer);
    public byte[] ToBytes()
    {
        MemoryStream stream = new MemoryStream();
        BinaryWriter writer = new BinaryWriter(stream);
        Save(ref writer);
        writer.Close();
        return stream.GetBuffer();
    }
    public void FromBytes(byte[] buffer)
    {
        BinaryReader reader = new BinaryReader(new MemoryStream(buffer));
        Load(ref reader);
        reader.Close();
    }
    public static void WriteString(ref BinaryWriter writer, ref string str, int nCount)
    {
        byte[] buf = Encoding.Default.GetBytes(str);
        if (buf.Length >= nCount)
        {
            buf[nCount - 1] = 0;
            writer.Write(buf, 0, nCount);
        }
        else
        {
            byte[] fill = new byte[nCount - buf.Length];
            if (buf.Length > 0)
                writer.Write(buf);
            else
                fill[0] = 0;
            writer.Write(fill);
        }
    }
    public static string ReadString(ref BinaryReader reader, int nCount)
    {
        byte[] buf = reader.ReadBytes(nCount);
        int nIdx = 0;
        while (nIdx < buf.Length && buf[nIdx] != 0)
        {
            ++nIdx;
        }
        Array.Resize(ref buf, nIdx);
        return Encoding.Default.GetString(buf);
    }
}

public class KProtocolHead : IProtocol, ISerial
{
    private UInt16 wProtocol;
    public UInt16 wServer;
    public KProtocolHead()
    {
        wProtocol = 0;
        wServer = 0;
    }
    public virtual UInt16 ProtocolId
    {
        get { return wProtocol; }
        set { wProtocol = value; }
    }

    public UInt16 ServerID
    {
        get { return wServer; }
        set { wServer = value; }
    }
    public virtual ISubProtocol GetSubProtocol()
    {
        return null;
    }
    public virtual void Load(ref BinaryReader reader)
    {
        wProtocol = reader.ReadUInt16();
        wServer = reader.ReadUInt16();
    }
    public virtual void Save(ref BinaryWriter writer)
    {
        writer.Write(wProtocol);
        writer.Write(wServer);
    }
    public virtual byte[] ToBytes()
    {
        MemoryStream stream = new MemoryStream();
        BinaryWriter writer = new BinaryWriter(stream);
        Save(ref writer);
        byte[] buffer = stream.GetBuffer();
        Array.Resize(ref buffer, (int)stream.Length);
        writer.Close();
        stream.Close();
        return buffer;
    }
    public virtual void FromBytes(byte[] buffer)
    {
        BinaryReader reader = new BinaryReader(new MemoryStream(buffer));
        Load(ref reader);
        reader.Close();
    }
};

public class KSubProtocolHead : KProtocolHead, ISubProtocol
{
    public UInt16 wSubProtocol;
    public UInt16 SubProtocolID
    {
        get {return wSubProtocol;}
        set {wSubProtocol = value;}
    }
    public override ISubProtocol GetSubProtocol()
    {
        return this;
    }
    public KSubProtocolHead()
    {
        wSubProtocol = 0;
        ProtocolId = (ushort)ProtocolDef.e2l_header_def;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        wSubProtocol = reader.ReadUInt16();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(wSubProtocol);
    }
};

public class e2g_openfile : KProtocolHead
{
    public byte bFlag;
    public byte bFullPath;
    public string szFileName;
    public e2g_openfile()
    {
        bFlag = 0;
        bFullPath = 0;
        szFileName = string.Empty;
        ProtocolId = (ushort)ProtocolDef.e2g_openfile_def;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        bFlag = reader.ReadByte();
        bFullPath = reader.ReadByte();
        szFileName = KSerial.ReadString(ref reader, 256);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(bFlag);
        writer.Write(bFullPath);
        KSerial.WriteString(ref writer, ref szFileName, 256);
    }
};

public class g2e_openfile : KProtocolHead
{
	public uint nFileLen;
	public int  nRetCode;
	public g2e_openfile()
	{
		nFileLen = 0;
		nRetCode = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nFileLen = reader.ReadUInt32();
        nRetCode = reader.ReadInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nFileLen);
        writer.Write(nRetCode);
    }
};

public class e2g_readfile : KProtocolHead
{
	public UInt16 nDataLen;
	public e2g_readfile()
	{
		nDataLen = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nDataLen = reader.ReadUInt16();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nDataLen);
    }
};

public class g2e_readfile : KProtocolHead
{
    public int      nRetCode;
	public byte[]   szBuf;
	public g2e_readfile()
	{
		nRetCode = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nRetCode = reader.ReadInt32();
        UInt32 nLen = reader.ReadUInt32();
        szBuf = reader.ReadBytes((int)nLen);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nRetCode);
        writer.Write((UInt32)szBuf.Length);
        writer.Write(szBuf);
    }
};

public class e2g_writefile : KProtocolHead
{
	public byte[] szBuf;
	public e2g_writefile(int nLen)
	{
        ProtocolId = (UInt16)ProtocolDef.e2g_writefile_def;
        szBuf = new byte[nLen];
	}
    public e2g_writefile(byte[] buffer)
    {
        FromBytes(buffer);
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        UInt16 nDataLen = reader.ReadUInt16();
        szBuf = reader.ReadBytes(nDataLen);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write((UInt16)szBuf.Length);
        writer.Write(szBuf);
    }
};

public class g2e_writefile : KProtocolHead
{
	public uint nWritedLen;
	public int nRetCode;
	public g2e_writefile()
	{
		nWritedLen = 0;
		nRetCode = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nWritedLen = reader.ReadUInt32();
        nRetCode = reader.ReadInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nWritedLen);
        writer.Write(nRetCode);
    }
};

public class e2g_seekfile : KProtocolHead
{
	public byte bKeep;
	public uint nOffset;
	public e2g_seekfile()
	{
        ProtocolId = (UInt16)ProtocolDef.e2g_seekfile_def;
		bKeep = 0;
		nOffset = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        bKeep = reader.ReadByte();
        nOffset = reader.ReadUInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(bKeep);
        writer.Write(nOffset);
    }
};

public class g2e_seekfile : KProtocolHead
{
	public int nRetCode;
	public g2e_seekfile()
	{
		nRetCode = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nRetCode = reader.ReadInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nRetCode);
    }
};

public class g2e_closefile : KProtocolHead
{
	public int nRetCode;
    public g2e_closefile()
    {
        nRetCode = 0;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nRetCode = reader.ReadInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nRetCode);
    }
};

public class cpubaseinfo : KSerial
{
	public string szCPUName;
	public string szCPUVendor;
	public string szCPUDesc;
	public int nCPULoad;
	public cpubaseinfo()
	{
		szCPUName = string.Empty;
		szCPUVendor = string.Empty;
		szCPUDesc = string.Empty;
		nCPULoad = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        szCPUName   = KSerial.ReadString(ref reader, 100);
        szCPUVendor = KSerial.ReadString(ref reader, 100);
        szCPUDesc = KSerial.ReadString(ref reader, 100);
        nCPULoad    = reader.ReadInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        KSerial.WriteString(ref writer, ref szCPUName, 100);
        KSerial.WriteString(ref writer, ref szCPUVendor, 100);
        KSerial.WriteString(ref writer, ref szCPUDesc, 100);
        writer.Write(nCPULoad);
    }
};

public class g2e_getcpubaseinfo : KProtocolHead
{
	public UInt16 nCPUCount;
	public cpubaseinfo[] Processor;
	public g2e_getcpubaseinfo()
	{
		nCPUCount = 0;
		Processor = new cpubaseinfo[10];
		for(int i = 0; i < 10; i++)
		{
			Processor[i] = new cpubaseinfo();
		}
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nCPUCount = reader.ReadUInt16();
        for (int i = 0; i < 10; ++i)
        {
            Processor[i].Load(ref reader);
        }
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nCPUCount);
        for (int i = 0; i < 10; ++i)
        {
            Processor[i].Save(ref writer);
        }
    }
};

public class g2e_getcpuload : KProtocolHead
{
	public UInt16 nCPUCount;
	public UInt16[] nLoad;
	public g2e_getcpuload()
	{
		nCPUCount = 0;
		nLoad = new UInt16[10];
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nCPUCount = reader.ReadUInt16();
        for (int i = 0; i < 10; ++i)
            nLoad[i] = reader.ReadUInt16();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nCPUCount);
        for (int i = 0; i < 10; ++i)
            writer.Write(nLoad[i]);
    }
};

public class g2e_getmeminfo : KProtocolHead
{
	public uint nTotalMem;
	public uint nFreeMem;
	public g2e_getmeminfo()
	{
		nTotalMem = 0;
		nFreeMem = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nTotalMem = reader.ReadUInt32();
        nFreeMem = reader.ReadUInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nTotalMem);
        writer.Write(nFreeMem);
    }
};

public class diskbaseinfo : KSerial
{
	public string szDiskDesc;
	public uint nTotalSize;
	public uint nFreeSize;
	public diskbaseinfo()
	{
		szDiskDesc = string.Empty;
		nTotalSize = 0;
		nFreeSize = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        szDiskDesc = KSerial.ReadString(ref reader, 100);
        nTotalSize = reader.ReadUInt32();
        nFreeSize = reader.ReadUInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        KSerial.WriteString(ref writer, ref szDiskDesc, 100);
        writer.Write(nTotalSize);
        writer.Write(nFreeSize);
    }
};

public class g2e_getdiskinfo : KProtocolHead
{
	public UInt16 nDiskCount;
	public diskbaseinfo[] Disk;
	public g2e_getdiskinfo()
	{
		nDiskCount = 0;
		Disk = new diskbaseinfo[10];
		for(int i = 0; i < 10; i++)
		{
			Disk[i] = new diskbaseinfo();
		}
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nDiskCount = reader.ReadUInt16();
        for (int i = 0; i < 10; ++i)
            Disk[i].Load(ref reader);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nDiskCount);
        for (int i = 0; i < 10; ++i)
            Disk[i].Save(ref writer);
    }
};

public class netcardinfo : KSerial
{
	public string szCardDesc;
	public string szIP;
	public string szMask;
	public string szMac;
	public netcardinfo()
	{
		szCardDesc = string.Empty;
		szIP = string.Empty;
		szMask = string.Empty;
		szMac = string.Empty;
	}
    public override void Load(ref BinaryReader reader)
    {
        szCardDesc  = KSerial.ReadString(ref reader, 100);
        szIP        = KSerial.ReadString(ref reader, 20);
        szMask      = KSerial.ReadString(ref reader, 20);
        szMac       = KSerial.ReadString(ref reader, 20);
    }
    public override void Save(ref BinaryWriter writer)
    {
        KSerial.WriteString(ref writer, ref szCardDesc, 100);
        KSerial.WriteString(ref writer, ref szIP, 20);
        KSerial.WriteString(ref writer, ref szMask, 20);
        KSerial.WriteString(ref writer, ref szMac, 20);
    }
};

public class g2e_getnetinfo : KProtocolHead
{
	public UInt16 nCardCount;
	public netcardinfo[] Card;
	public string szSystemName;
	public string szHostName;
	public g2e_getnetinfo()
	{
		nCardCount = 0;
		Card = new netcardinfo[10];
		for(int i = 0; i < 10; i++)
		{
			Card[i] = new netcardinfo();
		}
		szSystemName = string.Empty;
		szHostName = string.Empty;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nCardCount = reader.ReadUInt16();
        for (int i = 0; i < 10; ++i)
            Card[i].Load(ref reader);
        szSystemName = KSerial.ReadString(ref reader, 100);
        szHostName = KSerial.ReadString(ref reader, 100);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nCardCount);
        for (int i = 0; i < 10; ++i)
            Card[i].Save(ref writer);
        KSerial.WriteString(ref writer, ref szSystemName, 100);
        KSerial.WriteString(ref writer, ref szHostName, 100);
    }
};

public class netcardload : KSerial
{
	public uint nTXSize;
	public uint nRXSize;
	public uint nTXRate;
	public uint nRXRate;
	public netcardload()
	{
		nTXSize = 0;
		nRXSize = 0;
		nTXRate = 0;
		nRXRate = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        nTXSize = reader.ReadUInt32();
        nRXSize = reader.ReadUInt32();
        nTXRate = reader.ReadUInt32();
        nRXRate = reader.ReadUInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        writer.Write(nTXSize);
        writer.Write(nRXSize);
        writer.Write(nTXRate);
        writer.Write(nRXRate);
    }
};

public class g2e_getcardload : KProtocolHead
{
	public UInt16 nCardCount;
	public netcardload[] Card;
	public g2e_getcardload()
	{
		nCardCount = 0;
		Card = new netcardload[10];
		for(int i = 0; i < 10; i++)
		{
			Card[i] = new netcardload();
		}
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nCardCount = reader.ReadUInt16();
        for (int i = 0; i < 10; ++i)
            Card[i].Load(ref reader);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nCardCount);
        for (int i = 0; i < 10; ++i)
            Card[i].Save(ref writer);
    }
};

public class procinfo : KSerial
{
	public string szProcName;
	public uint nPID;
	public uint nMemUse;
	public uint nVMSize;
	public uint nCPUTime;
	public uint nThreadCount;

	public procinfo()
	{
		szProcName = string.Empty;
		nPID = 0;
		nMemUse = 0;
		nVMSize = 0;
		nCPUTime = 0;
		nThreadCount = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        szProcName  = KSerial.ReadString(ref reader, 50);
        nPID        = reader.ReadUInt32();
        nMemUse     = reader.ReadUInt32();
        nVMSize     = reader.ReadUInt32();
        nCPUTime    = reader.ReadUInt32();
        nThreadCount= reader.ReadUInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        KSerial.WriteString(ref writer, ref szProcName, 50);
        writer.Write(nPID);
        writer.Write(nMemUse);
        writer.Write(nVMSize);
        writer.Write(nCPUTime);
        writer.Write(nThreadCount);
    }
};

public class g2e_getprocinfo : KProtocolHead
{
	public UInt16 nProcCount;
	public procinfo[] Proc;
	public g2e_getprocinfo()
	{
		nProcCount = 0;
		Proc = new procinfo[80];
		for(int i = 0; i < 80; i++)
		{
			Proc[i] = new procinfo();
		}
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nProcCount = reader.ReadUInt16();
        for (int i = 0; i < 80; ++i)
            Proc[i].Load(ref reader);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nProcCount);
        for (int i = 0; i < 80; ++i)
            Proc[i].Save(ref writer);
    }
};

public class pluginfo : KSerial
{
	public string szPath;
	public string szModHAndModE;
	public string szGUID;
	public string szAuthor;
	public string szVersion;
	public string szDesc;
	public pluginfo()
	{
		szPath = string.Empty;
		szModHAndModE = string.Empty;
		szGUID = string.Empty;
		szAuthor = string.Empty;
		szVersion = string.Empty;
		szDesc = string.Empty;
	}
    public override void Load(ref BinaryReader reader)
    {
        szPath        = KSerial.ReadString(ref reader, 50); 
        szModHAndModE = KSerial.ReadString(ref reader, 50); 
        szGUID      　= KSerial.ReadString(ref reader, 50); 
        szAuthor      = KSerial.ReadString(ref reader, 50); 
        szVersion     = KSerial.ReadString(ref reader, 50); 
        szDesc      　= KSerial.ReadString(ref reader, 100); 
    }
    public override void Save(ref BinaryWriter writer)
    {
        KSerial.WriteString(ref writer, ref szPath, 50);
        KSerial.WriteString(ref writer, ref szModHAndModE, 50);
        KSerial.WriteString(ref writer, ref szGUID, 50);
        KSerial.WriteString(ref writer, ref szAuthor, 50);
        KSerial.WriteString(ref writer, ref szVersion, 50);
        KSerial.WriteString(ref writer, ref szDesc, 100);
    }
};

public class g2e_getpluginfo : KProtocolHead
{
	public int PlugCount;
	public pluginfo[] PlugInfo;
	public g2e_getpluginfo()
	{
		PlugCount = 0;
		PlugInfo = new pluginfo[5];
		for(int i = 0; i < 5; i++)
		{
			PlugInfo[i] = new pluginfo();
		}
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        PlugCount = reader.ReadInt32();
        for (int i = 0; i < 5; ++i)
            PlugInfo[i].Load(ref reader);

    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(PlugCount);
        for (int i = 0; i < 5; ++i)
            PlugInfo[i].Save(ref writer);
    }
};

public class e2g_exesql : KProtocolHead
{
	public uint     nSessionID;
	public string   szDatabase;
	public byte[]   szSql;
	public e2g_exesql()
	{
        ProtocolId  = (UInt16)ProtocolDef.e2g_exesql_def;
		nSessionID  = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nSessionID  = reader.ReadUInt32();
        szDatabase  = KSerial.ReadString(ref reader, 48);
        uint nSqlLen = reader.ReadUInt32();
        szSql = reader.ReadBytes((int)nSqlLen);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nSessionID);
        KSerial.WriteString(ref writer, ref szDatabase, 48);
        writer.Write((uint)szSql.Length);
        writer.Write(szSql);
    }
};

// 执行SQL语句：如果第一个SQL语句执行不成功则执行第二个SQL语句
public class e2g_exesql2 : KProtocolHead
{
    public uint nSessionID;
    public string szDatabase;
    public byte[] szSql1;
    public byte[] szSql2;
    public e2g_exesql2()
    {
        ProtocolId = (UInt16)ProtocolDef.e2g_exesql2_def;
        nSessionID = 0;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nSessionID = reader.ReadUInt32();
        szDatabase = KSerial.ReadString(ref reader, 48);
        UInt32 nSql1Len = reader.ReadUInt32();
        UInt32 nSql2Len = reader.ReadUInt32();
        szSql1 = reader.ReadBytes((int)nSql1Len);
        szSql2 = reader.ReadBytes((int)nSql2Len);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nSessionID);
        KSerial.WriteString(ref writer, ref szDatabase, 48);
        UInt32 nSql1Len = (UInt32)szSql1.Length;
        UInt32 nSql2Len = (UInt32)szSql2.Length;
        writer.Write(nSql1Len);
        writer.Write(nSql2Len);
        writer.Write(szSql1, 0, (int)nSql1Len);
        writer.Write(szSql2, 0, (int)nSql2Len);
    }
}

public class g2e_exesql : KProtocolHead
{
    public uint nSessionID;
    public uint nRetCode;
    public byte[] szResult;
    public g2e_exesql()
    {
        nSessionID = 0;
        nRetCode = 0;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nSessionID = reader.ReadUInt32();
        nRetCode = reader.ReadUInt32();
        UInt32 nLen = reader.ReadUInt32();
        szResult = reader.ReadBytes((int)nLen);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nSessionID);
        writer.Write(nRetCode);
        UInt32 nLen = (UInt32)szResult.Length;
        writer.Write(nLen);
        writer.Write(szResult);
    }
};

public class e2g_exesyscmd : KProtocolHead
{
    public UInt16 bNeedOutput;
    public string Command;
    public string InputBuff;
    public e2g_exesyscmd()
    {
        ProtocolId = (UInt16)ProtocolDef.e2g_exesyscmd_def;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        UInt16 bNeedOutput = reader.ReadUInt16();
        UInt16 nCmdLen = reader.ReadUInt16();
        UInt16 nBufLen = reader.ReadUInt16();
        if (nCmdLen > 0)
            Command = KSerial.ReadString(ref reader, nCmdLen + 1);
        if (nBufLen > 0)
            InputBuff = KSerial.ReadString(ref reader, nBufLen + 1);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        UInt16 nCmdLen = (UInt16)(Command.Length + 1);
        UInt16 nBufLen = (UInt16)(InputBuff.Length + 1);
        writer.Write(bNeedOutput);
        writer.Write(nCmdLen);
        writer.Write(nBufLen);
        KSerial.WriteString(ref writer, ref Command, nCmdLen);
        KSerial.WriteString(ref writer, ref InputBuff, nBufLen);
    }
};

public class g2e_exesyscmd : KProtocolHead
{
	public int ReturnCode;
	public string OutputBuff;
	public g2e_exesyscmd()
	{
		ReturnCode = 0;
		OutputBuff = string.Empty;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        ReturnCode = reader.ReadInt32();
        OutputBuff = KSerial.ReadString(ref reader, 256);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(ReturnCode);
        KSerial.WriteString(ref writer, ref OutputBuff, 256);
    }
};

public class e2l_SayToWorld : KSubProtocolHead
{
    public string Message;
	public e2l_SayToWorld()
	{
        ProtocolId = (UInt16)ProtocolDef.e2l_header_def;
        SubProtocolID = (UInt16)ProtocolDef.e2l_SayToWorld_def;
		Message = string.Empty;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        Message = KSerial.ReadString(ref reader, 256);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        KSerial.WriteString(ref writer, ref Message, 256);
    }
};

public class l2e_GetBasicInfo : KSubProtocolHead
{
	public UInt16 PlayerCount;
	public UInt16 UpTime;
	public l2e_GetBasicInfo()
	{
        ProtocolId = (UInt16)ProtocolDef.e2l_header_def;
        SubProtocolID = (UInt16)ProtocolDef.l2e_GetBasicInfo_def;
		PlayerCount = 0;
		UpTime = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        PlayerCount = reader.ReadUInt16();
        UpTime = reader.ReadUInt16();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(PlayerCount);
        writer.Write(UpTime);
    }
};

//public class e2l_ExeGMCmd : KProtocolHead
//{
//    public UInt16 RoleOrAccount;
//    public UInt16 GCorGS;
//    public string PlayerName;
//    public string Command;
//    public e2l_ExeGMCmd()
//    {
//        ProtocolId = (UInt16)ProtocolDef.e2l_ExeGMCmd_def;// (UInt16)ProtocolDef.e2l_header_def;
//        //SubProtocolID = (UInt16)ProtocolDef.e2l_ExeGMCmd_def;
//        PlayerName = string.Empty;
//        Command = string.Empty;
//        RoleOrAccount = 0;
//        GCorGS = 1;
//    }
//    public override void Load(ref BinaryReader reader)
//    {
//        base.Load(ref reader);
//        RoleOrAccount = reader.ReadUInt16();
//        GCorGS = reader.ReadUInt16();
//        PlayerName = KSerial.ReadString(ref reader, 32);
//        Command = KSerial.ReadString(ref reader, 1024);
//    }
//    public override void Save(ref BinaryWriter writer)
//    {
//        base.Save(ref writer);
//        writer.Write(RoleOrAccount);
//        writer.Write(GCorGS);
//        KSerial.WriteString(ref writer, ref PlayerName, 32);
//        KSerial.WriteString(ref writer, ref Command, 1024);
//    }
//};

//public class l2e_ExeGMCmd : KSubProtocolHead
//{
//    public int ReturnCode;
//    public l2e_ExeGMCmd()
//    {
//        ReturnCode = 0;
//    }
//    public override void Load(ref BinaryReader reader)
//    {
//        base.Load(ref reader);
//        ReturnCode = reader.ReadInt32();
//    }
//    public override void Save(ref BinaryWriter writer)
//    {
//        base.Save(ref writer);
//        writer.Write(ReturnCode);
//    }
//};


public class e2l_ExeGMCmd : KProtocolHead
{
    //新增字段，会话ID，用于从hashtable里取消息
    public uint nSessionID;
    public UInt16 RoleOrAccount;
    public UInt16 GCorGS;
	public string PlayerName;
	public string Command;
    public ArrayList GSList;
    public e2l_ExeGMCmd()
	{
        ProtocolId = (UInt16)ProtocolDef.e2l_ExeGMCmd_def;// (UInt16)ProtocolDef.e2l_header_def;
        //SubProtocolID = (UInt16)ProtocolDef.e2l_ExeGMCmd_def;
		PlayerName = string.Empty;
		Command = string.Empty;
        RoleOrAccount = 0;
        GCorGS = 1;
        GSList = new ArrayList();
        nSessionID = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);

        nSessionID = reader.ReadUInt32();

        RoleOrAccount = reader.ReadUInt16();
        GCorGS = reader.ReadUInt16();
        PlayerName = KSerial.ReadString(ref reader, 32);
        Command = KSerial.ReadString(ref reader, 1024);
        int nCount = reader.ReadInt32();
        for (int i=0; i < nCount ; i ++)
        {
            GSList.Add(reader.ReadUInt16());
        }
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nSessionID);
        writer.Write(RoleOrAccount);
        writer.Write(GCorGS);
        KSerial.WriteString(ref writer, ref PlayerName, 32);
        KSerial.WriteString(ref writer, ref Command, 1024);
        writer.Write(GSList.Count);
        foreach(UInt16 gs in GSList )
        {
            writer.Write(gs);
        }
    }
};

public class l2e_ExeGMCmd : KSubProtocolHead
{
    //新增字段，会话ID，用于从hashtable里取消息
    public uint nSessionID;
    public int ReturnCode;

    //新增字段，GM指令的执行结果
    public string szResult;

    public l2e_ExeGMCmd()
    {        
        ReturnCode = 0;
        nSessionID = 0;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);

        nSessionID = reader.ReadUInt32();

        ReturnCode = reader.ReadInt32();

        UInt32 nLen = reader.ReadUInt32();
        szResult = KSerial.ReadString(ref reader, (int)nLen);


    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nSessionID);
        writer.Write(ReturnCode);

        
        byte[] resultBytes = Encoding.Default.GetBytes(szResult);
        
        //UInt32 nLen = (UInt32)szResult.Length;
        UInt32 nLen = (UInt32)resultBytes.Length;
        
        //writer.Write(szResult.ToCharArray());
        writer.Write(nLen);
        
        writer.Write(resultBytes);
    }
};


public class e2g_Authentication : KProtocolHead
{
	public int X;
	public e2g_Authentication()
	{
        ProtocolId = (UInt16)ProtocolDef.e2g_Authentication_def;
		X = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        X = reader.ReadInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(X);
    }
};

public class g2e_Authentication : KProtocolHead
{
	public string Y;
	public g2e_Authentication()
	{
		Y = string.Empty;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        Y = KSerial.ReadString(ref reader, 64);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        KSerial.WriteString(ref writer, ref Y, 64);
    }
};

public enum KE_GUARD_PLAYERCOUNT
{
    emKGUARD_PLAYERCOUNT_TOTOAL = 0,		// 总的玩家人数
    emKGUARD_PLAYERCOUNT_OFFLIVE,			// 离线托管人数
    emKGUARD_PLAYERCOUNT_COUNT
};

// ahpho
public class e2l_update_custom_info : KSubProtocolHead
{

};
public class l2e_update_custom_info : KSubProtocolHead
{
    public int iInfoLen;
    public string strInfo;
    public l2e_update_custom_info()
    {
        iInfoLen = 0;
        strInfo = string.Empty;
    }
    public string[] GetInfoStrs()
    {
        return strInfo.Split(new char[] { '\n' });
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        iInfoLen = reader.ReadInt32();
        if (iInfoLen <= 0) return;
        byte[] rawdata = reader.ReadBytes(iInfoLen);
        strInfo = Encoding.Default.GetString(rawdata);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(iInfoLen);
        if (iInfoLen <= 0) return;
        byte[] byteArray = Encoding.Default.GetBytes(strInfo);
        writer.Write(byteArray);
    }
};

public class l2e_PlayerCount : KSubProtocolHead
{
    public int[] aryPlayerCount; // 数组最大数目为emKPLAYERCOUNT_COUNT，如果人数为-1表示未知
	public l2e_PlayerCount()
	{
        aryPlayerCount = new int[(int)KE_GUARD_PLAYERCOUNT.emKGUARD_PLAYERCOUNT_COUNT];
        for (int i = 0; i < (int)KE_GUARD_PLAYERCOUNT.emKGUARD_PLAYERCOUNT_COUNT; ++i)
        {
            aryPlayerCount[i] = -1;
        }
	}
    // 获取总在线人数
    public int GetTotalCount()
    {
        return aryPlayerCount[(int)KE_GUARD_PLAYERCOUNT.emKGUARD_PLAYERCOUNT_TOTOAL];
    }
    // 获取离线托管人数
    public int GetOffliveCount()
    {
        return aryPlayerCount[(int)KE_GUARD_PLAYERCOUNT.emKGUARD_PLAYERCOUNT_OFFLIVE];
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        UInt16 uCount = reader.ReadUInt16();
        for (int i = 0; i < uCount; ++i)
        {
            int nCount = reader.ReadInt32();
            if (i < (int)KE_GUARD_PLAYERCOUNT.emKGUARD_PLAYERCOUNT_COUNT)
            {
                aryPlayerCount[i] = nCount;
            }
        }
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        UInt16 uCount = (UInt16)aryPlayerCount.Length;
        writer.Write(uCount);
        for (int i = 0; i < (int)uCount; ++i)
        {
            writer.Write(aryPlayerCount[i]);
        }
    }
};

public class g2e_GetGuardDir : KProtocolHead
{
	public string GuardDir;
	public g2e_GetGuardDir()
	{
		GuardDir = string.Empty;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        GuardDir = KSerial.ReadString(ref reader, 256);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        KSerial.WriteString(ref writer, ref GuardDir, 256);
    }
};

public class e2l_Who : KSubProtocolHead
{
	public UInt16 Offset;
	public e2l_Who()
	{
        ProtocolId = (UInt16)ProtocolDef.e2l_header_def;
        SubProtocolID = (UInt16)ProtocolDef.e2l_Who_def;
		Offset = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        Offset = reader.ReadUInt16();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(Offset);
    }
};

public class l2e_Who_PlayerInfo : KSerial
{
	public string Name;

	public l2e_Who_PlayerInfo()
	{
		Name = string.Empty;
	}
    public override void Load(ref BinaryReader reader)
    {
        Name = KSerial.ReadString(ref reader, 32);
    }
    public override void Save(ref BinaryWriter writer)
    {
        KSerial.WriteString(ref writer, ref Name, 32);
    }
};

public class l2e_Who : KSubProtocolHead
{
	public UInt16 PlayerCount;
	public l2e_Who_PlayerInfo[] PlayerList;
	public l2e_Who()
	{
        ProtocolId = (UInt16)ProtocolDef.e2l_header_def;
        SubProtocolID = (UInt16)ProtocolDef.l2e_Who_def;
		PlayerCount = 0;
		PlayerList = new l2e_Who_PlayerInfo[20];
		for(int i = 0; i < 20; i++)
		{
			PlayerList[i] = new l2e_Who_PlayerInfo();
		}
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        PlayerCount = reader.ReadUInt16();
        for (int i = 0; i < 20; ++i)
            PlayerList[i].Load(ref reader);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(PlayerCount);
        for (int i = 0; i < 20; ++i)
            PlayerList[i].Save(ref writer);
    }
};

public class e2l_GetGlobalVariable : KSubProtocolHead
{
	public UInt16 VariableIndex;
	public e2l_GetGlobalVariable()
	{
        ProtocolId = (UInt16)ProtocolDef.e2l_header_def;
        SubProtocolID = (UInt16)ProtocolDef.e2l_GetGlobalVariable_def;
		VariableIndex = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        VariableIndex = reader.ReadUInt16();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(VariableIndex);
    }
};

public class l2e_GetGlobalVariable : KProtocolHead
{
	public UInt16 VariableIndex;
	public int VariableValue;
	public l2e_GetGlobalVariable()
	{
		VariableIndex = 0;
		VariableValue = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        VariableIndex = reader.ReadUInt16();
        VariableValue = reader.ReadInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(VariableIndex);
        writer.Write(VariableValue);
    }
};

public class e2l_SetGlobalVariable : KSubProtocolHead
{
	public UInt16 VariableIndex;
	public int VariableValue;
	public e2l_SetGlobalVariable()
	{
        ProtocolId = (UInt16)ProtocolDef.e2l_header_def;
        SubProtocolID = (UInt16)ProtocolDef.e2l_SetGlobalVariable_def;
		VariableIndex = 0;
		VariableValue = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        VariableIndex = reader.ReadUInt16();
        VariableValue = reader.ReadInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(VariableIndex);
        writer.Write(VariableValue);
    }
};

public class e2g_Key : KSerial
{
	public byte[] Data;
	public e2g_Key()
	{
		Data = new byte[128];
	}
    public override void Load(ref BinaryReader reader)
    {
        Data = reader.ReadBytes(128);
    }
    public override void Save(ref BinaryWriter writer)
    {
        writer.Write(Data);
    }
};

public class e2g_ConfigInfo : KSerial
{
	public int X;
	public string Y;
	public UInt16 ConfigFileDataLength;
	public string ConfigFileData;

	public e2g_ConfigInfo()
	{
		X = 0;
		Y = string.Empty;
		ConfigFileDataLength = 0;
		ConfigFileData = string.Empty;
	}
    public override void Load(ref BinaryReader reader)
    {
        X = reader.ReadInt32();
        Y = KSerial.ReadString(ref reader, 64);
        ConfigFileDataLength = reader.ReadUInt16();
        ConfigFileData = KSerial.ReadString(ref reader, 4096);
    }
    public override void Save(ref BinaryWriter writer)
    {
        writer.Write(X);
        KSerial.WriteString(ref writer, ref Y, 64);
        writer.Write(ConfigFileDataLength);
        KSerial.WriteString(ref writer, ref ConfigFileData, 4096);
    }
};

public class e2g_config : KProtocolHead
{
	public e2g_Key Key;
	public byte[] ConfigInfo;

	public e2g_config()
	{
        ProtocolId = (UInt16)ProtocolDef.e2g_config_def;
		Key = new e2g_Key();
		ConfigInfo = new byte[4166];
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        Key.Load(ref reader);
        ConfigInfo = reader.ReadBytes(4166);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        Key.Save(ref writer);
        writer.Write(ConfigInfo);
    }
};

public class e2g_file : KProtocolHead
{
	public UInt16 wFileLen;
	public string szPath;
    public string szData;
   
	public e2g_file()
	{
        ProtocolId = (UInt16)ProtocolDef.e2g_file_def;
		wFileLen = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        wFileLen = reader.ReadUInt16();
        szPath = KSerial.ReadString(ref reader, 256);
        szData = KSerial.ReadString(ref reader, 4096);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(wFileLen);
        KSerial.WriteString(ref writer, ref szPath, 256);
        KSerial.WriteString(ref writer, ref szData, 4096);
    }
};

public class e2g_switchmode : KProtocolHead
{
	public UInt16 Mode;
	public e2g_switchmode()
	{
        ProtocolId = (UInt16)ProtocolDef.e2g_switchmode_def;
		Mode = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        Mode = reader.ReadUInt16();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(Mode);
    }
};

public class l2e_GetGameStartTime : KSubProtocolHead
{
	public string GameStartTime;

	public l2e_GetGameStartTime()
	{
        SubProtocolID = (UInt16)ProtocolDef.l2e_GetGameStartTime_def;
		GameStartTime = string.Empty;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        GameStartTime = KSerial.ReadString(ref reader, 32);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        KSerial.WriteString(ref writer, ref GameStartTime, 32);
    }
};

public class l2e_ReportError : KSubProtocolHead
{
	public int Module;
	public int ErrorCode;
	public l2e_ReportError()
	{
		Module = 0;
		ErrorCode = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        Module = reader.ReadInt32();
        ErrorCode = reader.ReadInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(Module);
        writer.Write(ErrorCode);
    }
};

public class e2g_DeliverKey : KProtocolHead
{
	public e2g_Key Key;
	public e2g_DeliverKey()
	{
        ProtocolId = (UInt16)ProtocolDef.e2g_DeliverKey_def;
		Key = new e2g_Key();
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        Key.Load(ref reader);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        Key.Save(ref writer);
    }
};

public class e2g_exesyscmd_large : KProtocolHead
{
    public UInt16 bNeedOutput;
	public string Command;
	public string InputBuff;
	public e2g_exesyscmd_large()
	{
        ProtocolId = (UInt16)ProtocolDef.e2g_exesyscmd_large_def;
        bNeedOutput = 1;
		Command = string.Empty;
		InputBuff = string.Empty;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        bNeedOutput = reader.ReadUInt16();
        Command = KSerial.ReadString(ref reader, 6144);
        InputBuff = KSerial.ReadString(ref reader, 256);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(bNeedOutput);
        KSerial.WriteString(ref writer, ref Command, 6144);
        KSerial.WriteString(ref writer, ref InputBuff, 256);
    }
};

public class l2e_info : KSubProtocolHead
{
	public string Info;
	public l2e_info()
	{
		Info = string.Empty;
	}
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        KSerial.WriteString(ref writer, ref Info, 1024);
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        Info = KSerial.ReadString(ref reader, 1024);
    }
};

public class l2e_info_large : KSubProtocolHead
{
	public string InfoLarge;
	public l2e_info_large()
	{
		InfoLarge = string.Empty;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        InfoLarge = KSerial.ReadString(ref reader, 4096);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        KSerial.WriteString(ref writer, ref InfoLarge, 4096);
    }
};

public class l2e_getpid : KProtocolHead
{
    public int nPid;
    public l2e_getpid()
    {
        nPid = 0;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nPid = reader.ReadInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nPid);
    }
};

public class e2g_updateserver : KProtocolHead
{
    public string m_strPack;     	// 压缩包名.默认路径为guard下的temp
    public string m_strMD5;     	// MD5,有这个参数时更新前先校验
    public byte m_nCheck;		    // 0=更新，1＝仅MD5校验
    public e2g_updateserver()
    {
        ProtocolId = (ushort)ProtocolDef.e2g_updateserver_def;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        m_strPack = KSerial.ReadString(ref reader, 128);
        m_strMD5 = KSerial.ReadString(ref reader, 33);
        m_nCheck = reader.ReadByte();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        KSerial.WriteString(ref writer, ref m_strPack, 128);
        KSerial.WriteString(ref writer, ref m_strMD5, 33);
        writer.Write(m_nCheck);
    }
}

public class e2g_ftpdownload : KProtocolHead
{
    public string m_strFtpHost;     // FTP服务器IP或者域名 192.168.20.198
    public string m_strFtpUser;     // ftp帐号
    public string m_strFtpPass;     // ftp密码
    public string m_strFtpPath;     // ftp路径，带压缩包名的完整路径.e.g. update/up.rar
    public e2g_ftpdownload()
    {
        ProtocolId = (ushort)ProtocolDef.e2g_ftpdownload_def;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        m_strFtpHost = KSerial.ReadString(ref reader, 128);
        m_strFtpUser = KSerial.ReadString(ref reader, 32);
        m_strFtpPass = KSerial.ReadString(ref reader, 32);
        m_strFtpPath = KSerial.ReadString(ref reader, 256);
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        KSerial.WriteString(ref writer, ref m_strFtpHost, 128);
        KSerial.WriteString(ref writer, ref m_strFtpUser, 32);
        KSerial.WriteString(ref writer, ref m_strFtpPass, 32);
        KSerial.WriteString(ref writer, ref m_strFtpPath, 256);
    }
}

public class g2e_updateserver : KProtocolHead
{
	public UInt16 m_nCurrentStep;	// 当前步骤（从1开始）
	public UInt16 m_nStepCount;	    // 总步骤数
	public UInt16 m_nResult;		// 处理结果，1为成功，0为失败
    public String m_strMessage;     // 附带消息
    public g2e_updateserver()
    {
        m_nCurrentStep = 0;
        m_nStepCount = 0;
        m_nResult = 0;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        m_nCurrentStep = reader.ReadUInt16();
        m_nStepCount   = reader.ReadUInt16();
        m_nResult      = reader.ReadUInt16(); 
        UInt16 nMsgLen = reader.ReadUInt16();
        if (nMsgLen > 0)
        {
            m_strMessage = KSerial.ReadString(ref reader, nMsgLen);
        }
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(m_nCurrentStep);
        writer.Write(m_nStepCount);
        writer.Write(m_nResult);
        writer.Write((UInt16)m_strMessage.Length);
        if (m_strMessage.Length > 0)
        {
            KSerial.WriteString(ref writer, ref m_strMessage, m_strMessage.Length);
        }
    }
}

public class g2e_ftpdownload : KProtocolHead
{
    public byte m_nProgress;      // 当前已下载百分比（返回值0～100）e.g 12表示12%
    public byte m_nResult;		    // 处理结果，1为成功，0为失败
    public String m_strMessage;     // 附带消息;结束时返回MD5
    public g2e_ftpdownload()
    {
        m_nProgress = 0;
        m_nResult = 0;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        m_nProgress = reader.ReadByte();
        m_nResult = reader.ReadByte();
        UInt16 nMsgLen = reader.ReadUInt16();
        if (nMsgLen > 0)
        {
            m_strMessage = KSerial.ReadString(ref reader, nMsgLen);
        }
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(m_nProgress);
        writer.Write(m_nResult);
        writer.Write((UInt16)m_strMessage.Length);
        if (m_strMessage.Length > 0)
        {
            KSerial.WriteString(ref writer, ref m_strMessage, m_strMessage.Length);
        }
    }
}

public class e2g_server_pid : KProtocolHead
{
    public e2g_server_pid()
    {
        ProtocolId = (ushort)ProtocolDef.e2g_serverpid_def;
    }    
}

public class g2e_server_pid : KProtocolHead
{
    public int nRetCode;
    public g2e_server_pid()
	{
		nRetCode = -2;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nRetCode = reader.ReadInt32();        
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nRetCode);
    }
}

public class e2g_groupVer : KProtocolHead
{
    public e2g_groupVer()
    {
        ProtocolId = (ushort)ProtocolDef.e2g_groupVer_def;
    }
}

public class g2e_groupVer : KProtocolHead
{
	public int		nMajor;
	public int		nMinor;
	public int		nRevision;
	public int		nBuild;
    public g2e_groupVer()
	{
		nMajor = 0;
		nMinor = 0;
		nRevision = 0;
		nBuild = 0;
	}
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
		nMajor = reader.ReadInt32();
		nMinor = reader.ReadInt32();
		nRevision = reader.ReadInt32();
		nBuild = reader.ReadInt32();
    }    
};

public class e2g_serverid : KProtocolHead
{
    public Int32    nGoddess;
    public Int32    nBishop;
    public Int32    nGameCenter;
    public Int32    nLogServer;
    public Int32    nGameServer01;
    public Int32    nGameServer02;
    public Int32    nGameServer03;
    public Int32    nGameServer04;
    public Int32    nGameServer05;
    public Int32    nGameServer06;
    public Int32    nGameServer07;
    public Int32    nGameServer08;
    public Int32    nGameServer09;
    public Int32    nGameServer10;
    public Int32    nGameServer11;
    public Int32    nGameServer12;
    public e2g_serverid()
    {
        ProtocolId = (ushort)ProtocolDef.e2g_serverid_def;
        nGoddess = 0;
        nBishop = 0;
        nGameCenter = 0;
        nLogServer = 0;
        nGameServer01 = 0;
        nGameServer02 = 0;
        nGameServer03 = 0;
        nGameServer04 = 0;
        nGameServer05 = 0;
        nGameServer06 = 0;
        nGameServer07 = 0;
        nGameServer08 = 0;
        nGameServer09 = 0;
        nGameServer10 = 0;
        nGameServer11 = 0;
        nGameServer12 = 0;
    }
    public override void Load(ref BinaryReader reader)
    {
        base.Load(ref reader);
        nGoddess = reader.ReadInt32();
        nBishop = reader.ReadInt32();
        nGameCenter = reader.ReadInt32();
        nLogServer = reader.ReadInt32();
        nGameServer01 = reader.ReadInt32();
        nGameServer02 = reader.ReadInt32();
        nGameServer03 = reader.ReadInt32();
        nGameServer04 = reader.ReadInt32();
        nGameServer05 = reader.ReadInt32();
        nGameServer06 = reader.ReadInt32();
        nGameServer07 = reader.ReadInt32();
        nGameServer08 = reader.ReadInt32();
        nGameServer09 = reader.ReadInt32();
        nGameServer10 = reader.ReadInt32();
        nGameServer11 = reader.ReadInt32();
        nGameServer12 = reader.ReadInt32();
    }
    public override void Save(ref BinaryWriter writer)
    {
        base.Save(ref writer);
        writer.Write(nGoddess);
        writer.Write(nBishop);
        writer.Write(nGameCenter);
        writer.Write(nLogServer);
        writer.Write(nGameServer01);
        writer.Write(nGameServer02);
        writer.Write(nGameServer03);
        writer.Write(nGameServer04);
        writer.Write(nGameServer05);
        writer.Write(nGameServer06);
        writer.Write(nGameServer07);
        writer.Write(nGameServer08);
        writer.Write(nGameServer09);
        writer.Write(nGameServer10);
        writer.Write(nGameServer11);
        writer.Write(nGameServer12);
    }
}

public enum ProtocolDef {
	e2l_header_def = 0,
    l2e_header_def =1,
	g2e_ping_def =2,
	e2g_ping_def =3,
	e2g_openfile_def=4,
	g2e_openfile_def=5,
	e2g_readfile_def=6,
	g2e_readfile_def=7,
	e2g_writefile_def=8,
	g2e_writefile_def=8,
	e2g_seekfile_def=10,
	g2e_seekfile_def=11,
	e2g_closefile_def=12,
	g2e_closefile_def=13,
	e2g_getcpubaseinfo_def=14,
	g2e_getcpubaseinfo_def=15,
	e2g_getcpuload_def=16,
	g2e_getcpuload_def=17,
	e2g_getmeminfo_def=18,
	g2e_getmeminfo_def=19,
	e2g_getdiskinfo_def=20,
	g2e_getdiskinfo_def=21,
	e2g_getnetbaseinfo_def=22,
	g2e_getnetinfo_def=23,
	e2g_getcardload_def=24,
	g2e_getcardload_def=25,
	e2g_getprocinfo_def=26,
	g2e_getprocinfo_def=27,
	e2g_getpluginfo_def=28,
	g2e_getpluginfo_def=29,
	e2g_exesql_def=30,
	g2e_exesql_def=31,
	e2g_exesyscmd_def=32,
	g2e_exesyscmd_def=33,
	e2l_SayToWorld_def=34,
	e2l_GetBasicInfo_def=35,
	l2e_GetBasicInfo_def=36,
	e2l_ExeGMCmd_def=37,
	l2e_ExeGMCmd_def=38,
	e2g_Authentication_def=39,
	g2e_Authentication_def=40,
	e2l_PlayerCount_def=41,
	l2e_PlayerCount_def=42,
	e2g_GetGuardDir_def=43,
	g2e_GetGuardDir_def=44,
	e2g_UpdateGuard_def=45,
	e2l_Who_def=46,
	l2e_Who_def=47,
	e2l_GetGlobalVariable_def=48,
	l2e_GetGlobalVariable_def=49,
	e2l_SetGlobalVariable_def=50,
	e2g_config_def=51,
	e2g_file_def=52,
	g2e_config_def=53,
	e2g_switchmode_def=54,
	g2e_switchmode_def=55,
	e2l_GetGameStartTime_def=56,
	l2e_GetGameStartTime_def=57,
	l2e_ReportError_def=58,
	e2g_DeliverKey_def=59,
	e2g_exesyscmd_large_def=60,
	l2e_info_def=61,
	l2e_info_large_def=62,
	e2l_exit_def=63,
	e2l_getpid_def=64,			    // 获取进程ID，struct: KProtocolHead
	l2e_getpid_def=65,			    // 获取进程ID，struct: l2e_getpid
	l2e_ping_def=66,			    // server发给web的ping
    e2g_exesql2_def=67,			    // 执行SQL语句：如果第一个SQL语句执行不成功则执行第二个SQL语句
    e2g_updateserver_def=68,		// 更新服务器命令
    g2e_updateserver_def=69,		// 更新服务器结果
    c2r_gmcmd_def=70,				// 由gmc向gamecenter(relay)发送请求协议
    r2c_gmcmd_def=71,

    //e2l_gmcmd_def = 72,					// 执行GM命令
    e2l_gmcmd_gs_def = 72,				// 在GameServer上执行GM命令
    e2l_gmcmd_gc_def = 73,				// 在GameCenter上执行GM命令
    l2e_send_gm_message_def = 74,        // 给GM返回一个字符串消息

    l2e_gmcmd_def = 75,					// 返回GM命令结果
    l2e_gmchn_def = 76,					// GM频道信息
    e2l_queryrolelist_def = 77,		// 查询角色列表
    l2e_rolelist_def = 78,				// 角色列表
    e2g_ftpdownload_def = 79,			// 更新服务器命令
    g2e_ftpdownload_def = 80,
    e2g_updateself_def = 81,				// 更新guard
    p2g_log_def = 82,				// python脚本给guard发送的日志协议
    e2g_serverid_def = 83,				// 各个服务器对应的servrerid
    g2l_header_def = 84,					// guard给server发送的协议
    l2g_header_def = 85,					// server给guard发送的协议
    e2g_serverpid_def = 86,				// 请求server的进程ID
	g2e_serverpid_def = 87,			// 应答server的进程ID    
    g2e_groupVer_def = 88,           // 应答某组服务的版本号
    e2g_groupVer_def = 89,          // 获取某组服务的版本号
    e2l_update_custom_info_def = 90,     // Web向客户进程请求更新自定义显示信息
    l2e_update_custom_info_def = 91,     // 客户进程向Web发送自定义显示信息

    fseye_protocol_count
};

public enum FSEyeResult {
	fseye_success = 0,
	guard_err,
	guard_create_client_err,
	guard_startup_client_err,
	guard_client_invalidhadle,
	guard_client_send_err,
	plug_opendll_err,
	plug_getproc_err,
	plug_creat_err,
	filetran_opening_err,
	filetran_app_err,
	filetran_cre_err,
	filetran_seek_err,
	filetran_close_err,
	mydb_err_opendb,
	mydb_err_query,
	mydb_err_dbuncon,
	servicestate_stopped,
	servicestate_starting,
	servicestate_running,
	servicestate_stopping,
	servicestate_restarting,
	db_err,
	db_transaction_started_err,
	db_transaction_not_started_err,
	db_rebuild_table_err,
	db_delete_table_err,
	db_table_not_exist_err,
	db_table_exist_err,
	db_begin_transaction_err,
	db_commit_transaction_err,
	db_rollback_transaction_err,
	db_get_table_data_err,
	db_adapter_not_init_err,
	db_add_table_data_err,
	db_update_table_data_err,
	as_err,
	as_bad_argument_err,
	as_user_not_exist_err,
	as_user_already_login_err,
	as_fm_task_complete,
	sec_err,
	sec_allow,
	sec_deny,
	sec_unknown,
	sec_not_found_in_cache,
	sec_not_enough_privilege_err,
	sec_ace_already_exist_err,
	sec_ace_not_exist_err,
	sec_user_already_login_err,
	sec_user_not_exist_err,
	sec_user_not_login_err,
	l2e_ExeGMCmd_err,
	l2e_ExeGMCmd_player_not_found_err,
	g2e_ExeSysCmd_done,
	g2e_ExeSysCmd_busy,
	g2e_ExeSysCmd_result,
	mydb_more_result,
};

public class ResultInfo {
	public static string[] Description = new string[] {
		"成功",
		"guard一般性错误",
		"网络库生成client失败",
		"网络库调用startup失败",
		"网络库无效句柄",
		"网络库发送失败",
		"打开插件动态库失败",
		"取得生成函数地址失败",
		"生成插件接口失败",
		"文件已经打开",
		"续传文件不存在",
		"创建文件失败",
		"文件重置失败",
		"文件已经关闭",
		"mydb插件打开数据库错误",
		"mydb插件查询错误",
		"mydb插件数据库连接断开",
		"游戏服务已经关闭",
		"游戏服务正在启动",
		"游戏服务正在运行",
		"游戏服务正在关闭",
		"游戏服务正在重启",
		"db一般性错误",
		"事务已经开始",
		"事务尚未开始",
		"重新建表失败",
		"删除表失败",
		"表不存在",
		"表已经存在",
		"开始事务失败",
		"提交事务失败",
		"回滚事务失败",
		"获得表数据失败",
		"适配器没有初始化",
		"添加表数据失败",
		"更新表数据失败",
		"管理服务器一般错误",
		"参数错误",
		"用户不存在失败",
		"用户已经登录失败",
		"文件管理任务完成",
		"安全一般错误",
		"允许",
		"拒绝",
		"未知",
		"没有在缓存中找到",
		"权限不够错误",
		"ACE已经存在错误",
		"ACE不存在错误",
		"用户已经登录失败",
		"用户已经登录失败",
		"用户尚未登录失败",
		"执行GM指令失败",
		"执行GM指令失败（没有找到指定的玩家）",
		"执行系统指令完成",
		"已有系统指令在执行",
		"系统指令中间结果",
		"数据库查询：还有更多的结果数据",
	};
};

