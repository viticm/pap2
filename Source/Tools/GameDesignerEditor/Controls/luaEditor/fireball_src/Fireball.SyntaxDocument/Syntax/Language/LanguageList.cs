using System;
using System.IO;
using System.Collections.Generic;
using System.Collections;

namespace Fireball.Syntax
{
	/// <summary>
	/// Language list class
	/// </summary>
	public class LanguageList: IEnumerable
	{

		Dictionary<string,Language> _Languages=new Dictionary<string,Language>();


		/// <summary>
		/// Empty languagelist
		/// </summary>
        public LanguageList()
        {
           
        }


        /// <summary>
        /// Istance a new LanguageList from a directory and load files with specified extension
        /// </summary>
        /// <param name="directory">Directory where search files</param>
        /// <param name="ext">extension of syntax files</param>
        public LanguageList(string directory, string ext)
        {
            

            string[] files = Directory.GetFiles(directory, ext); //"*.syn");

            foreach (string file in files)
            {
                SyntaxLoader l = new SyntaxLoader();
                Language lng = l.Load(file);

                if (!_Languages.ContainsKey(lng.Name))
                    _Languages.Add(lng.Name, lng);
            }
        }



        public Language GetFromIndex(int index)
        {
            if (index >= 0 && index < _Languages.Count)
            {
                string[] keys = new string[_Languages.Keys.Count];

                _Languages.Keys.CopyTo(keys, 0);

                return _Languages[ keys[index]];
            }

            return null;
        }

        /// <summary>
        /// Get a language from it's name
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public Language GetLanguageFromName(string name)
        {
            if (!_Languages.ContainsKey(name))
                return null;

            return _Languages[name];
        }

        public void Add(Language lang)
        {
            if (!_Languages.ContainsKey(lang.Name))
                _Languages.Add(lang.Name, lang);
        }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		public Language GetLanguageFromFile(string path)
		{
			string extension= System.IO.Path.GetExtension (path);

			foreach (Fireball.Syntax.Language lang in _Languages.Values)
			{
				foreach (Fireball.Syntax.FileType ft in lang.FileTypes)
				{
					if (extension.ToLower () == ft.Extension.ToLower ())
					{
						return lang;
					}
				}
			}
			return null;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
        public System.Collections.IEnumerator ListLanguages()
		{
			return _Languages.GetEnumerator() ;
		}

        #region IEnumerable Membri di

        public IEnumerator GetEnumerator()
        {
            return _Languages.GetEnumerator(); 
        }

        #endregion
    }
}
