using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using UnityEngine;
public class owner_parser : MonoBehaviour {

    string url = "https://maplestory.nexon.com/Ranking/World/Total?c=%EB%A9%94%EC%A3%BC%ED%96%A5";
    string datapath = "C:/Users/seong/Desktop/projects_OSG/projects_OSG/personal/maple_check/";
    StringReader strReader;
    string ranktable;
    bool parse_flag = false;
    // Use this for initialization
    void Start()
    {
        ranktable = null;
        Debug.Log("system init");
        StartCoroutine(Get_html());

    }

    void WriteData(string strData,string datapath, string dataname)
    {
        FileStream f = new FileStream(datapath + dataname, FileMode.CreateNew, FileAccess.Write);
        StreamWriter writer = new StreamWriter(f, System.Text.Encoding.Unicode);
        writer.WriteLine(strData);
        writer.Close();
    }

    public string Parse()
    {
        string aLine, aParagraph = null;

        aLine = strReader.ReadLine();

        if (aLine == null)
            return null;

        aParagraph = aParagraph + aLine + " ";
        aParagraph = aParagraph + "\n";

        //Debug.Log("in Parsing process..");
        return aLine;
    }

        IEnumerator Get_html()
    {
        WWW html_data = new WWW(url);
        do
        {
            yield return null;
        }
        while (!html_data.isDone);

        if (html_data.error!=null)
        {
            Debug.Log("web.error"+html_data.error);
            yield break;
        }
        else
        {
            Debug.Log("net_connected");
            WriteData(html_data.text, datapath, "data.txt");
            strReader = new StringReader(html_data.text);
        }
    }

    // Update is called once per frame
    void Update () {

        if (strReader == null)
            return;

        string now_line;
        now_line = Parse();

        if (now_line == null)
            return;

        if (now_line.IndexOf("rank_table_wrap") > -1)
        {
            Debug.Log("detected");
            parse_flag = true;
        }
        //if (now_line.IndexOf("rank_table_wrap") < 0)
        //    return;
        if(parse_flag)
            ranktable = ranktable + now_line + "\n"; ;

        if (now_line.IndexOf("<!-- 종합랭킹 : End -->") > -1)
        {
            Debug.Log("1st parse complete");
            WriteData(ranktable, datapath, "1st parse.txt");
            parse_flag = false;
        }
    }
}
