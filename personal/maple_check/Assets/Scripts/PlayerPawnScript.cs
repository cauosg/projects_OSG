using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class PlayerPawnScript : MonoBehaviour {
    public string character_name;
    public string url;
    public string data_path;
    public string parse_data_name;
    public int parse_state = 0;
    public WWW html_data;
    // Use this for initialization
    void Start () {
        url = "https://maplestory.nexon.com/Ranking/World/Total?c=";
        data_path = Application.dataPath + "/datafiles/";
        parse_data_name = "1st parse.txt";
        html_data = new WWW(url + character_name);
        
    }

    // Update is called once per frame
    void Update () {
		
	}

    public void set_parse_state(int ind)
    {
        parse_state = ind;
    }
}
