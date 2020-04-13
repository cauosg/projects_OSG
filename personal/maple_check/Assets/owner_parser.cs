using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using UnityEngine;
using UnityEngine.Networking;
public class owner_parser : MonoBehaviour {

    string url = "https://maplestory.nexon.com/Ranking/World/Total?c=%EB%A9%94%EC%A3%BC%ED%96%A5";
    string datapath = "C:/Users/seong/Desktop/projects_OSG/projects_OSG/personal/maple_check/";
    UnityWebRequest request;
    XmlDocument doc;

    // Use this for initialization
    void Start()
    {
        Debug.Log("system init");
        StartCoroutine(WebRequest());

    }

    void WriteData(string strData,string datapath, string dataname)
    {
        FileStream f = new FileStream(datapath + dataname, FileMode.Append, FileAccess.Write);
        StreamWriter writer = new StreamWriter(f, System.Text.Encoding.Unicode);
        writer.WriteLine(strData);
        writer.Close();
    }

    public void Parse()
    {
        XmlWriterSettings xml_setting = new XmlWriterSettings();
        xml_setting.Indent = true;
        XmlWriter xml_writer = XmlWriter.Create(datapath + "data_xml.txt");
        doc = new XmlDocument();
        doc.LoadXml(request.downloadHandler.text.Trim());

        doc.WriteContentTo(xml_writer);
        xml_writer.Close();


        //StringReader sr = new StringReader(request.downloadHandler.text);
        //string src = sr.ReadLine();
        //string[] src_temp;

        ////while (src != null) {
        //int temp = src.IndexOf("rank_table");
        //Debug.Log(temp);
        //}
    }
        IEnumerator WebRequest()
    {
        request = new UnityWebRequest();
        using (request = UnityWebRequest.Get(url))
        {
            yield return request.SendWebRequest();
            if (request.isNetworkError)
            {
                Debug.Log(request.error);
            }
            else
            {
                //Debug.Log(request.downloadHandler.text);
                var results = request.downloadHandler.text;
                WriteData(results,datapath, "Data.txt");
                Debug.Log(results);
                Parse();
            }
        }
    }

    // Update is called once per frame
    void Update () {
		
	}
}
