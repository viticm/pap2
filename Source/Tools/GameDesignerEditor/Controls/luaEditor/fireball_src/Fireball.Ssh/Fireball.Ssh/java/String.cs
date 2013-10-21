#region Licences
//    Copyright (C) 2005  Sebastian Faltoni <sebastian@dotnetfireball.net>
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#endregion Licences

using System;
using Text = System.Text;
using Str = System.String;

namespace Fireball.Ssh.java
{
	/// <summary>
	/// Summary description for String.
	/// </summary>
	public class String
	{
		string s;
		public String(string s)
		{
			this.s=s;
		}

		public String(object o):this(o.ToString())
		{
		}

		public String(byte[] arr):this(getString(arr))
		{
		}

		public String(byte[] arr, int offset, int len):this(getString(arr, offset, len))
		{
		}

		public static implicit operator String (string s1) 
		{
			if(s1==null) return null;
			return new String(s1);
		}

		public static implicit operator Str (String s1) 
		{
			if(s1==null) return null;
			return s1.ToString();
		}

		public static Fireball.Ssh.java.String operator+(Fireball.Ssh.java.String s1, Fireball.Ssh.java.String s2)
		{
			return new Fireball.Ssh.java.String(s1.ToString()+s2.ToString());
		}

		public byte[] getBytes()
		{
			return String.getBytes(this);
		}

		public override string ToString()
		{
			return s;
		}

		public String toLowerCase()
		{
			return this.ToString().ToLower();
		}

		public bool startsWith(string prefix)
		{
			return this.ToString().StartsWith(prefix);
		}

		public int indexOf(string sub)
		{
			return this.ToString().IndexOf(sub);
		}

		public String substring(int start, int len)
		{
			return this.ToString().Substring(start, len);
		}

		public String subString(int start, int len)
		{
			return substring(start, len);
		}

		public String substring(int len)
		{
			return this.ToString().Substring(len);
		}

		public String subString(int len)
		{
			return substring(len);
		}

		public int Length()
		{
			return this.ToString().Length;
		}

		public bool equals(object o)
		{
			return this.ToString().Equals(o.ToString());
		}

		public override bool Equals(object obj)
		{
			return this.equals (obj);
		}

		public override int GetHashCode()
		{
			return s.GetHashCode ();
		}

		public static string getString(byte[] arr)
		{
			return getString(arr, 0, arr.Length);
		}

		public static string getString(byte[] arr, int offset, int len)
		{
			return Text.Encoding.Default.GetString(arr, offset, len);
		}

		public static string getStringUTF8(byte[] arr)
		{
			return getStringUTF8(arr, 0, arr.Length);
		}

		public static string getStringUTF8(byte[] arr, int offset, int len)
		{
			return Text.Encoding.UTF8.GetString(arr, offset, len);
		}

		public static byte[] getBytes(string str)
		{
			return Text.Encoding.Default.GetBytes( str );
		}
		public static byte[] getBytesUTF8(string str)
		{
			return Text.Encoding.UTF8.GetBytes( str );
		}
	}
}
