using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class PlayerPawnScript : MonoBehaviour {
    public string character_name;
    public string url;
    public string data_path;
    public string parse_data_name;

    public string level;
    public string job;
    public string pop;
    public string char_rank;
    public string tot_stat;
    public string arc;
    public string exp;
    public string guild;

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

    //public void set_(string in_string)
    //{
    //    = in_string;
    //}

    //public string get_()
    //{
    //    return ;
    //}
    public string get_char_name()
    {
        return character_name;
    }

    public void set_level(string in_string)
    {
        level = in_string;
    }
    public string get_level()
    {
        return level;
    }

    public void set_job(string in_string)
    {
        job = in_string;
    }
    public string get_job()
    {
        return job;
    }

    public void set_pop(string in_string)
    {
        pop = in_string;
    }
    public string get_pop()
    {
        return pop;
    }

    public void set_char_rank(string in_string)
    {
        char_rank = in_string;
    }
    public string get_char_rank()
    {
        return char_rank;
    }

    public void set_tot_stat(string in_string)
    {
        tot_stat = in_string;
    }
    public string get_tot_stat()
    {
        return tot_stat;
    }

    public void set_arc(string in_string)
    {
        arc = in_string;
    }
    public string get_arc()
    {
        return arc;
    }

    public void set_exp(string in_string)
    {
        exp = in_string;
    }
    public string get_exp()
    {
        return exp;
    }

    public void set_guild(string in_string)
    {
        guild = in_string;
    }
    public string get_guild()
    {
        return guild;
    }
}
