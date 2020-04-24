using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using System.Xml;


public class class_parser : MonoBehaviour {

    string character_name, datapath, url, dataname;
    string ranktable;
    StreamReader reader;
    string now_line;
    int line_index;
    // Use this for initialization
    void Start () {

        Init();
        InitReadData();
    }

    void Init()
    {
        datapath = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().data_path;
        dataname = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().parse_data_name;
        character_name = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().character_name;
        url = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().url;
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

    public void Parameters()
    {

    }

    string Classifier(int index)
    {
        string out_data = null;

        switch (index) {
            case 7://char avatar url

                break;
            case 12://소속,직업

                break;
            case 15://레벨

                break;
            case 16://경치

                break;
            case 17://인기도

                break;
            case 18://길드

                break;

            default:break;
        }

        return out_data;
    }

    // Update is called once per frame
    void Update () {
        bool state_checker = 2 == GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().parse_state;

        if(state_checker)
            now_line = reader.ReadLine();
	}
}
