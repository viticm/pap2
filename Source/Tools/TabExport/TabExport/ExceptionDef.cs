using System;
using System.Collections.Generic;
using System.Text;

namespace TabExport
{
	class ExportException : ApplicationException
	{
		public readonly string fileName;
		public ExportException(string f, string msg) :
			base(msg)
		{
			fileName = f;
		}
	}


}
