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
using System.IO;

namespace Fireball.Ssh.jsch
{
	/* -*-mode:java; c-basic-offset:2; -*- */
	/*
	Copyright (c) 2002,2003,2004 ymnk, JCraft,Inc. All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice,
		this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright 
		notice, this list of conditions and the following disclaimer in 
		the documentation and/or other materials provided with the distribution.

	3. The names of the authors may not be used to endorse or promote products
		derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JCRAFT,
	INC. OR ANY CONTRIBUTORS TO THIS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
	OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
	EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	*/

	public class JSch{

		static System.Collections.Hashtable config;
	
		public static void Init()
		{
			config=new System.Collections.Hashtable();

			//  config.Add("kex", "diffie-hellman-group-exchange-sha1");
			config.Add("kex", "diffie-hellman-group1-sha1,diffie-hellman-group-exchange-sha1");
			config.Add("server_host_key", "ssh-rsa,ssh-dss");
			//config.Add("server_host_key", "ssh-dss,ssh-rsa");

//			config.Add("cipher.s2c", "3des-cbc,blowfish-cbc");
//			config.Add("cipher.c2s", "3des-cbc,blowfish-cbc");

			config.Add("cipher.s2c", "3des-cbc,aes128-cbc");
			config.Add("cipher.c2s", "3des-cbc,aes128-cbc");

//			config.Add("mac.s2c", "hmac-md5,hmac-sha1,hmac-sha1-96,hmac-md5-96");
//			config.Add("mac.c2s", "hmac-md5,hmac-sha1,hmac-sha1-96,hmac-md5-96");
			config.Add("mac.s2c", "hmac-md5");
			config.Add("mac.c2s", "hmac-md5");
			config.Add("compression.s2c", "none");
			config.Add("compression.c2s", "none");
			config.Add("lang.s2c", "");
			config.Add("lang.c2s", "");

			config.Add("diffie-hellman-group-exchange-sha1", 
				"Fireball.Ssh.jsch.DHGEX");
			config.Add("diffie-hellman-group1-sha1", 
				"Fireball.Ssh.jsch.DHG1");

			config.Add("dh",            "Fireball.Ssh.jsch.jce.DH");
			config.Add("3des-cbc",      "Fireball.Ssh.jsch.jce.TripleDESCBC");
			//config.Add("blowfish-cbc",  "Fireball.Ssh.jsch.jce.BlowfishCBC");
			config.Add("hmac-sha1",     "Fireball.Ssh.jsch.jce.HMACSHA1");
			config.Add("hmac-sha1-96",  "Fireball.Ssh.jsch.jce.HMACSHA196");
			config.Add("hmac-md5",      "Fireball.Ssh.jsch.jce.HMACMD5");
			config.Add("hmac-md5-96",   "Fireball.Ssh.jsch.jce.HMACMD596");
			config.Add("sha-1",         "Fireball.Ssh.jsch.jce.SHA1");
			config.Add("md5",           "Fireball.Ssh.jsch.jce.MD5");
			config.Add("signature.dss", "Fireball.Ssh.jsch.jce.SignatureDSA");
			config.Add("signature.rsa", "Fireball.Ssh.jsch.jce.SignatureRSA");
			config.Add("keypairgen.dsa",   "Fireball.Ssh.jsch.jce.KeyPairGenDSA");
			config.Add("keypairgen.rsa",   "Fireball.Ssh.jsch.jce.KeyPairGenRSA");
			config.Add("random",        "Fireball.Ssh.jsch.jce.Random");

			config.Add("aes128-cbc",    "Fireball.Ssh.jsch.jce.AES128CBC");

			//config.Add("zlib",          "com.jcraft.jsch.jcraft.Compression");

			config.Add("StrictHostKeyChecking",  "ask");
		}
	
	internal Fireball.Ssh.java.util.Vector pool=new Fireball.Ssh.java.util.Vector();
	internal Fireball.Ssh.java.util.Vector identities=new Fireball.Ssh.java.util.Vector();
	//private KnownHosts known_hosts=null;
	private HostKeyRepository known_hosts=null;

	public JSch(){
		//known_hosts=new KnownHosts(this);
		if (config==null)
			Init();
	}

	public Session getSession(String username, String host)  { return getSession(username, host, 22); }
	public Session getSession(String username, String host, int port)  {
		Session s=new Session(this); 
		s.setUserName(username);
		s.setHost(host);
		s.setPort(port);
		pool.Add(s);
		return s;
	}

		internal bool removeSession(Session session)
		{
			lock(pool)
			{
				return pool.remove(session);
			}
		}

	public void setHostKeyRepository(HostKeyRepository foo){
		known_hosts=foo;
	}
	public void setKnownHosts(String foo) {
		if(known_hosts==null) known_hosts=new KnownHosts(this);
		if(known_hosts is KnownHosts){
		lock(known_hosts){
		((KnownHosts)known_hosts).setKnownHosts(foo); 
		}
		}
	}
	public void setKnownHosts(StreamReader foo) { 
		if(known_hosts==null) known_hosts=new KnownHosts(this);
		if(known_hosts is KnownHosts){
		lock(known_hosts){
		((KnownHosts)known_hosts).setKnownHosts(foo); 
		}
		}
	}
	/*
	HostKeyRepository getKnownHosts(){ 
		if(known_hosts==null) known_hosts=new KnownHosts(this);
		return known_hosts; 
	}
	*/
	public HostKeyRepository getHostKeyRepository(){ 
		if(known_hosts==null) known_hosts=new KnownHosts(this);
		return known_hosts; 
	}
	/*
	public HostKey[] getHostKey(){
		if(known_hosts==null) return null;
		return known_hosts.getHostKey(); 
	}
	public void removeHostKey(String foo, String type){
		removeHostKey(foo, type, null);
	}
	public void removeHostKey(String foo, String type, byte[] key){
		if(known_hosts==null) return;
		known_hosts.remove(foo, type, key); 
	}
	*/
	public void addIdentity(String foo) {
		addIdentity(foo, (String)null);
	}
	public void addIdentity(String foo, String bar) {
		Identity identity=new IdentityFile(foo, this);
		if(bar!=null) identity.setPassphrase(bar);
		identities.Add(identity);
	}
	internal String getConfig(String foo){ return (String)(config[foo]); }

	private System.Collections.ArrayList proxies;
	void setProxy(String hosts, Proxy proxy){
		String[] patterns=Util.split(hosts, ",");
		if(proxies==null){proxies=new System.Collections.ArrayList();}
		lock(proxies){
		for(int i=0; i<patterns.Length; i++){
		if(proxy==null){
		proxies[0] = null;
		proxies[0] = System.Text.Encoding.Default.GetBytes( patterns[i] );
		}
		else{
		proxies.Add( System.Text.Encoding.Default.GetBytes( patterns[i] ) );
		proxies.Add(proxy);
		}
		}
		}
	}
	internal Proxy getProxy(String host){
		if(proxies==null)return null;
		byte[] _host= System.Text.Encoding.Default.GetBytes( host );
		lock(proxies){
		for(int i=0; i<proxies.Count; i+=2){
		if(Util.glob(((byte[])proxies[i]), _host)){
		return (Proxy)(proxies[i+1]);
		}
		}
		}
		return null;
	}
	internal void removeProxy(){
		proxies=null;
	}

	public static void setConfig(System.Collections.Hashtable foo){
		lock(config){
			System.Collections.IEnumerator e=foo.Keys.GetEnumerator();
		while(e.MoveNext()){
		String key=(String)(e.Current);
		config.Add(key, (String)(foo[key]));
		}
		}
	}
	}

}
