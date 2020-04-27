using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class PlayerPawnScript : MonoBehaviour {
    public string character_name;
    public string url;
    public string data_path;
    public string parse_data_name;
    int parse_state = 0;
    WWW html_data;
    string char_image_url;
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


    //html
    public WWW get_html() {
        return html_data;
    }

    //state
    public void set_parse_state(int ind) {
        parse_state = ind;
    }
    public int get_parse_state() {
        return parse_state;
    }

    //image
    public void set_char_image(string in_url) {
        char_image_url = in_url;
    }
    public string get_char_image() {
        return char_image_url;
    }
}
