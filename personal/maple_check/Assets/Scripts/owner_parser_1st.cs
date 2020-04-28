using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
public class Owner_parser_1st : MonoBehaviour {


    string character_name, datapath, url, dataname;
    StringReader strReader;
    string ranktable;
    bool parse_flag, state_flag = false;
    // Use this for initialization

    void Start()
    {

    }

    void Init()
    {
        datapath = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().data_path;
        dataname = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().parse_data_name;
        character_name = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().character_name;
        url = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().url;

        string page_data = GameObject.Find("net_manager").GetComponent<Net_magnager>().page_data;
        strReader = new StringReader(page_data);
        ranktable = null;

        Debug.Log("dataPath : " + datapath);
        Debug.Log("system init");
    }

    void WriteData(string strData)
    {
        FileStream f = new FileStream(datapath + dataname, FileMode.Create, FileAccess.Write);
        StreamWriter writer = new StreamWriter(f, System.Text.Encoding.Unicode);
        writer.WriteLine(strData);
        writer.Close();
        f.Close();
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

    // Update is called once per frame
    void Update () {

        if (!state_flag)
        {
            if (GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_parse_state() == 1)
            {
                state_flag = true;
                Init();
                Debug.Log("state trasintted to 1");
            }
            else
                return;
        }


        //Debug.Log("on parse state");
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
            ranktable = ranktable + now_line + "\n";
            //Debug.Log((now_line.IndexOf("</tr>")));
            if (now_line.IndexOf("</tr>") > -1)
            {
                Debug.Log("1st parse complete");
                WriteData(ranktable);
                parse_flag = false;
                GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().set_parse_state(2);
                state_flag = false;
            }
        }
    }
}
