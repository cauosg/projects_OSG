using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Net;

public class Parser_3rd : MonoBehaviour {

    int state_marker = 4;
    string data_name = "3rd_parse.txt";
    string prev_url = "https://maplestory.nexon.com";
    string char_rank, page_data, data_path,char_name;
    bool state_flag = false;
    // Use this for initialization
    void Start () {
		
	}

    private void Init()
    {
        //char_rank = prev_url + WWW.EscapeURL(GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_char_rank());
        char_rank = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_char_rank();
        Debug.Log(WWW.UnEscapeURL(char_name));
        data_path = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().data_path;
        char_name = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_char_name();
    }

    void WriteData(string strData)
    {
        FileStream f = new FileStream(data_path + data_name, FileMode.Create, FileAccess.Write);
        StreamWriter writer = new StreamWriter(f, System.Text.Encoding.Unicode);
        writer.WriteLine(strData);
        writer.Close();
        f.Close();
    }
    string U_maple_solver() {
        string outstr = prev_url + char_rank;
        int ind_prev = outstr.IndexOf(char_name);
        //int ind_prev = outstr.IndexOf(WWW.EscapeURL(char_name));
        int ind_next = outstr.IndexOf("?p=mik");
        //Debug.Log(ind_prev);
        //Debug.Log(ind_next);

        string temp_a = outstr.Substring(0, ind_prev);
        string temp_b = outstr.Substring(ind_next, outstr.Length - ind_next);

        outstr = temp_a + WWW.UnEscapeURL(char_name) + temp_b;
        //Debug.Log(outstr.Substring(1, ind_prev - 1));
        //Debug.Log(outstr.Substring(ind_next, outstr.Length - ind_next));
        Debug.Log("generated url: "+outstr);

        return outstr;
    }
    IEnumerator Get_html()
    {

        //string url_merged = prev_url + char_rank;
        //////string url_merged = prev_url + WWW.UnEscapeURL(char_rank);
        //Debug.Log(url_merged);
        //U_maple_solver();
        ////Application.OpenURL(url_merged);
        //WWW html_data = new WWW(char_rank);// new WWW()
        ////html_data.
        string solved_url = U_maple_solver();
        Debug.Log("solved url: " + solved_url);
        WWW html_data = new WWW(solved_url);
        //WWW html_data = new WWW("https://maplestory.nexon.com/Common/Character/Detail/메주향?p=mikO8qgdC4hElCwBGQ6GOx8CmO11EvduZkV0bRbPAhaZsM3KeXVCwvz%2fe7wrLp5u5dQ9vaDGUBUCkGtINfQvzb92r7v%2b8ShdbaG8n5LPfBFfJcBOY2utM8IQ49VfKqXi1cXeNgwD6kisWSqf7W5DdbtTfxxGuqIBK%2fX2M3%2fsDYQ%3d");
        while (!html_data.isDone)
        {
            yield return null;
        }
        

        if (html_data.error != null)
        {
            Debug.Log("2nd web.error" + html_data.error);
            Debug.Log(html_data.isDone);
            Debug.Log(html_data.bytesDownloaded);
            Debug.Log(html_data.url);
            
            yield break;
        }
        else
        {
            Debug.Log("2nd net_connected");
            page_data = html_data.text;
            GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().set_parse_state(6);
            WriteData(page_data);
           // html_data.Dispose();
            yield break;
        }
    }

    // Update is called once per frame
    void Update () {
        if (!state_flag)
        {
            if (GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_parse_state() == state_marker)
            {
                state_flag = true;
                Init();
                Debug.Log("state trasintted to " + state_marker as string);
                Debug.Log((WWW.UnEscapeURL(char_name)));
                StartCoroutine(Get_html());
            }
            else
                return;
        }
        //Classifier();
    }
}
