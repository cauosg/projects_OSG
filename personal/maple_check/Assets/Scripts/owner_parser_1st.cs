using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
public class owner_parser_1st : MonoBehaviour {


    string character_name, datapath, url, dataname;
    StringReader strReader;
    string ranktable;
    bool parse_flag = false;
    // Use this for initialization

    void Start()
    {
        Init();

        Debug.Log("dataPath : " + datapath);
        Debug.Log("system init");
        StartCoroutine(Get_html());

    }

    void Init()
    {
        datapath = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().data_path;
        dataname = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().parse_data_name;
        character_name = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().character_name;
        url = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().url;

        ranktable = null;
    }

    void WriteData(string strData)
    {
        FileStream f = new FileStream(datapath + dataname, FileMode.Create, FileAccess.Write);
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
        WWW html_data = new WWW(url + character_name);
        do
        {
            yield return null;
        }
        while (!html_data.isDone);

        if (html_data.error!=null)
        {
            Debug.Log("web.error"+html_data.error);
            Debug.Log("\n nowurl : " + url + character_name);
            yield break;
        }
        else
        {
            Debug.Log("net_connected");
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

        if (now_line.IndexOf("r_round_search") > -1)
        {
            Debug.Log("detected");
            parse_flag = true;
        }
        //if (now_line.IndexOf("rank_table_wrap") < 0)
        //    return;
        if (parse_flag)
        {
            ranktable = ranktable + now_line + "\n"; ;

            if (now_line.IndexOf("<tr class=") > -1)
            {
                Debug.Log("1st parse complete");
                WriteData(ranktable);
                parse_flag = false;
            }
        }
    }
}
