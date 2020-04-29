using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Set_Texts : MonoBehaviour {

    public Text rank, totstat, arc, union, level, char_name, job, pop;

    bool state_flag,is_loaded = false;

	// Use this for initialization
	void Start () {
	
	}
	
    void Update_params()
    {
        //rank.text = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_char_rank();
        //totstat.text = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_tot_stat();
        //arc.text = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_arc();
        //union.text = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_
        char_name.text = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_char_name();
        job.text = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_job();
        pop.text = "인기도 " + GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_pop();
        level.text = "Lv." + GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_level();

    }


	// Update is called once per frame
	void Update () {
        if (!state_flag)
        {
            if (GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_parse_state() == 5)
            {
                state_flag = true;
                is_loaded = true;
                Debug.Log("state trasintted to 5");
            }
            else
                return;
        }
        if (is_loaded)
        {
            Update_params();
            is_loaded = false;
        }
    }
}
