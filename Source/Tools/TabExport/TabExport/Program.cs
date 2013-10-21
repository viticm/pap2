using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.IO;
using System.Text.RegularExpressions;

namespace TabExport
{
	class Program
	{
		static string connStr;
		static string basePath;

		static void Main(string[] args)
		{
			IniConfig config; 
			try {
				config = new IniConfig("TabExport.ini", "GB18030", false);
			}
			catch(FileNotFoundException) {
				Console.WriteLine("Config file {0} not found.", "TabExport.ini");
				return;
			}
			using (FileStream fs = new FileStream("err.log", FileMode.OpenOrCreate, FileAccess.Write))
			using (StreamWriter errSw = new StreamWriter(fs, Encoding.Default))
			{
				connStr = config.GetValue("base", "ConnStr");
				basePath = config.GetValue("Base", "BasePath", "");
				if(!Directory.Exists(basePath) && basePath!="") {
					Console.WriteLine("Error: folder not exist: \"{0}\"", basePath);
					errSw.WriteLine("Error: folder not exist: \"{0}\"", basePath);
					return;
				}

				
				using (SqlConnection conn = new SqlConnection(connStr))
				{
					try {
						fs.SetLength(0);
						foreach (KeyValuePair<string, string> pair in config.Items("Tables")) {
							string table = pair.Key;
							string outFile = pair.Value;
							if(outFile == "") {
								Console.WriteLine("Output file unknown: {0}", table);
								errSw.WriteLine("Output file unknown: {0}", table);
								Console.WriteLine();
								errSw.WriteLine();
								continue;
							}
							outFile = Path.Combine(basePath, outFile);
							outFile = outFile.Replace('/', '\\');
							
							Console.Write("Exporting {0}...", table);
							try {
								TabExporter.Export(conn, table, outFile, Encoding.GetEncoding("GB18030"));
							}
							catch(Exception e) {
								errSw.WriteLine(e.Message);
								errSw.WriteLine();
								Console.WriteLine(e.Message);
								Console.WriteLine();
								continue;
							}
							Console.WriteLine("success.");
						}
					}
					catch(KeyNotFoundException) {
						errSw.WriteLine("[Tables] not found in TabExport.ini");
						Console.WriteLine("[Tables] not found in TabExport.ini");
					}
					catch (Exception e)
					{
						errSw.WriteLine(e.Message);
						Console.WriteLine(e.Message);
					}
					
				}
			}
			//Console.ReadKey();
		}
	}
}
