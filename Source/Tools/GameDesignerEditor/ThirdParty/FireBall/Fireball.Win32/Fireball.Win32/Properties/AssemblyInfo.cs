#region Copyright
//    Copyright (C) 2005  Sebastian Faltoni
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
#endregion


using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Security;

// Le informazioni generali relative a un assembly sono controllate dal seguente 
// insieme di attributi. Per modificare le informazioni associate a un assembly
// occorre quindi modificare i valori di questi attributi.
[assembly: AssemblyTitle("Fireball.Win32")]
[assembly: AssemblyDescription("")]
[assembly: AssemblyConfiguration("")]
[assembly: AssemblyCompany("")]
[assembly: AssemblyProduct("Fireball.Win32")]
[assembly: AssemblyCopyright("Copyright © dotnetfireball.org 2006")]
[assembly: AssemblyTrademark("")]
[assembly: AssemblyCulture("")]

// Se si imposta ComVisible su false, i tipi in questo assembly non saranno visibili 
// ai componenti COM. Se è necessario accedere a un tipo in questo assembly da 
// COM, impostare su true l'attributo ComVisible per tale tipo.
[assembly: ComVisible(false)]

// Se il progetto viene esposto a COM, il GUID che segue verrà utilizzato per creare l'ID della libreria dei tipi
[assembly: Guid("67fdbdda-cd66-4300-a9f4-c477ae4b828d")]

// Le informazioni sulla versione di un assembly sono costituite dai seguenti quattro valori:
//
//      Numero di versione principale
//      Numero di versione secondario 
//      Numero build
//      Revisione
//
// È possibile specificare tutti i valori o impostare come predefiniti i valori Numero revisione e Numero build 
// utilizzando l'asterisco (*) come descritto di seguito:
#if DEBUG
[assembly: AssemblyVersion("2.0.0.10")]
[assembly: AssemblyFileVersion("2.0.0.10")]
#endif

[assembly: AllowPartiallyTrustedCallers()]