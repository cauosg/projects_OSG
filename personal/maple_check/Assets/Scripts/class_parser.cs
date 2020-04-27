using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using System.Xml;


public class Class_parser : MonoBehaviour {

    string character_name, datapath, url, dataname;
    string ranktable;
    string prev_url = "https://maplestory.nexon.com";
    //StringReader strReader;
    StreamReader reader;
    StreamWriter writer;
    int line_index;
    bool state_flag = false;
    FileStream f_in,f_out;
    // Use this for initialization
    void Start () {


    }

    void Init()
    {
        datapath = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().data_path;
        dataname = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().parse_data_name;
        character_name = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().character_name;
        url = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().url;
        f_in = new FileStream(datapath + dataname, FileMode.Open, FileAccess.Read);
        f_out = new FileStream(datapath + "parse_out.txt", FileMode.Create, FileAccess.ReadWrite);
        reader = new StreamReader(f_in, System.Text.Encoding.Unicode);
        writer = new StreamWriter(f_out, System.Text.Encoding.Unicode);
    }

    void InitReadData()
    {
        Debug.Log("readstart");
        Debug.Log(" " + datapath + dataname);
        //WWW html_data;
        //reader = new StreamReader(f, System.Text.Encoding.Unicode);
        line_index = 0;
        //return reader.ReadLine();
    }

    public void Record_data(string to_write, int begin_ind, int end_ind)
    {
        string subdata = to_write.Substring(begin_ind, end_ind - begin_ind);
        Debug.Log(subdata);
        subdata = subdata + '\n';
        writer.WriteLine(subdata);
    }

    public void Parameters()
    {

    }

    string Classifier()
    {
        line_index++;
        string now_line = reader.ReadLine();

        string out_data = null;
        int begin_ind, end_ind = 0;
        switch (line_index) {
            case 7://char avatar url
                begin_ind = now_line.IndexOf("src=") + 5;//4 + 1
                end_ind = now_line.IndexOf(".png") + 4;//4
                //
                Record_data(now_line, begin_ind, end_ind);

                string char_img_url = now_line.Substring(begin_ind, end_ind - begin_ind);
                //set char_img
                GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().set_char_image(char_img_url);
                break;

            case 11://랭킹데이터
                begin_ind = now_line.IndexOf("href=") + 6;//5 + 1
                end_ind = now_line.IndexOf(" target=") - 1;//-1
                Record_data(now_line, begin_ind, end_ind);
                break;

            case 12://소속,직업
                begin_ind = now_line.IndexOf("dd") + 3;//2 + 1
                //debug_ind = now_line.IndexOf(" / ");//
                end_ind = now_line.IndexOf("</dd>");//-2
                Record_data(now_line, begin_ind, end_ind);
                break;

            case 15://레벨
                begin_ind = now_line.IndexOf("Lv.") + 3;//2 + 1
                end_ind = now_line.IndexOf("</td>");//-2
                Record_data(now_line, begin_ind, end_ind);
                break;

            case 16://경치
                begin_ind = now_line.IndexOf("td") + 3;//2 + 1
                end_ind = now_line.IndexOf("</td>");//-2
                Record_data(now_line, begin_ind, end_ind);
                break;

            case 17://인기도
                begin_ind = now_line.IndexOf("td") + 3;//2 + 1
                end_ind = now_line.IndexOf("</td>");//-2
                Record_data(now_line, begin_ind, end_ind);
                break;

            case 18://길드
                begin_ind = now_line.IndexOf("td") + 3;//2 + 1
                end_ind = now_line.IndexOf("</td>");//-2
                Record_data(now_line, begin_ind, end_ind);



                reader.Close();
                writer.Close();
                f_in.Close();
                f_out.Close();
                state_flag = false;
                GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().set_parse_state(3);
                break;

            default:
                
                break;
        }

        return out_data;
    }

    // Update is called once per frame
    void Update () {
        if (!state_flag)
        {
            if (GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_parse_state() == 2)
            {
                state_flag = true;
                Init();
                InitReadData();
                Debug.Log("state trasintted to 2");
            }
            else
                return;
        }
        Classifier();
	}
}
