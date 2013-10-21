using System;
using System.Collections.Generic;
using System.Collections;
using System.Text;
using System.IO;

namespace TabExport
{
	public class IniKeyNotFoundException : ApplicationException
	{
		public readonly string Section;
		public readonly string Key;

		public IniKeyNotFoundException(string section, string key)
			:
			base(string.Format("Key: \"{0}\" under section \"{1}\" not found.", key, section))
		{
			Section = section;
			Key = key;
		}

	}

	public class IniConfig
	{
		const string SECTION_DELIMITER = "/";

		string m_iniFile;
		string m_encoding; 

		Dictionary<string, string> m_comments;
		Dictionary<string, Dictionary<string, string>> m_keyValue;
		Dictionary<string, int> m_rank;
		int m_rankID;
		string m_lastComment;
		StringComparer m_strComp;
		

		public IniConfig(string fileName, string encoding, bool caseSensitive)
		{
			m_iniFile = fileName;
			m_encoding = encoding;

			if(caseSensitive) {
				m_strComp = StringComparer.Ordinal;
			}
			else {
				m_strComp = StringComparer.OrdinalIgnoreCase;
			}

			m_comments = new Dictionary<string, string>(m_strComp);
			m_keyValue = new Dictionary<string, Dictionary<string, string>>(m_strComp);
			m_rank = new Dictionary<string,int>();
			m_lastComment = "";
			m_rankID = 0;
			Read(fileName, encoding);
		}

		public void Read(string fileName, string encoding)
		{
			using(FileStream fs = File.Open(fileName, FileMode.Open, FileAccess.Read))
			using(StreamReader sr = new StreamReader(fs, Encoding.GetEncoding(encoding)))
			{
				string comment = "";
				string section = "";
				string line = sr.ReadLine();
				while (line != null) {
					line = line.Trim();
					if (line.StartsWith("#")) {
						comment = comment == "" ? line : comment + "\n" + line;
					}
					else if (line.StartsWith("[") && line.EndsWith("]")) {
						section = line.Substring(1, line.Length -2);
						string key = "/" + section + "/";
						if (comment != "") {
							m_comments.Add(key, comment);
							comment = "";
						}
						m_rank.Add(key, m_rankID++);
					}
					else if (line != "") {
						ProcessKeyValue(line, section, comment);
						comment = "";
					}

					line = sr.ReadLine();
				}

				if (comment != "") {
					m_lastComment = comment;
				}
			}
			
		}

		public string GetValue(string section, string key)
		{
			if (section == "")
				section = "/";
			//string key1 = String.Format("/{0}/{1}", section, key);
			string val = "";
			
			try
			{
				val = m_keyValue[section][key];
			}
			catch (KeyNotFoundException)
			{
				throw new IniKeyNotFoundException(section, key);
			}

			return val;
		}

		public string GetValue(string section, string key, string defaultValue)
		{
			string val = defaultValue;
			try {
				val = GetValue(section, key);
			}
			catch(KeyNotFoundException) {

			}
			return val;
		}

		
		private void ProcessKeyValue(string line, string section, string comment)
		{
			int pos = line.IndexOf("=");
			string key = line.Substring(0, pos);
			string val = line.Substring(pos + 1);
			key = key.Trim();
			val = val.Trim();
			if (val.StartsWith("\"", StringComparison.Ordinal) && 
				val.EndsWith("\"", StringComparison.Ordinal)) {
					val = val.Substring(1, val.Length - 2);
			}

			string key2;

			if(section == "") {
				key2 = "/" + key;
			}
			else{
				key2 = String.Format("/{0}/{1}", section, key);
			}
			

			try {
				m_keyValue[section].Add(key, val);
			}
			catch(KeyNotFoundException) {
				m_keyValue.Add(section, new Dictionary<string, string>(m_strComp));
				m_keyValue[section].Add(key, val);
			}

			if (comment!="") {
				m_comments.Add(key2, comment);
			}
			m_rank.Add(key2,m_rankID++);
		}

		public IEnumerable<KeyValuePair<string, string>> Items(string section)
		{
			foreach(KeyValuePair<string, string> pair in m_keyValue[section]) {
				yield return pair;
			}
		}
	}
}
