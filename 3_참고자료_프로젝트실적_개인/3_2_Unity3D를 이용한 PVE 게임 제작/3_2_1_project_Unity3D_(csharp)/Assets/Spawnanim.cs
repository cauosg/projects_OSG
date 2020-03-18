using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Spawnanim : MonoBehaviour {


    private bool issample = false;
    public float Spawninterval = 2.0f;
    public float spd = 1.0f;
    public float maxheight = 5.0f;
    private float Spawncount = 0.0f;
    private bool rising, falling = false;
	// Use this for initialization
	void Start () {
        rising = true;
        if (transform.position.y < -50)
            issample = true;
	}
	
	// Update is called once per frame
	void Update () {
        if (issample) { return; }
        //if(Spawncount>0.1f)
        Spawncount += Time.deltaTime;

        //Debug.Log(Spawncount);

        if (rising)
            transform.Translate(Vector3.up * spd * Spawncount*Spawncount*Time.deltaTime);

        if (rising && transform.position.y > maxheight)
        {
            rising = false;
            falling = true;
            Spawncount = 0.0f;
        }
        if (falling)
        {
            transform.Translate(Vector3.down * spd * Spawncount * Spawncount * Time.deltaTime);
            if (Spawncount > Spawninterval)
            {
                //Debug.Log("Destroyed");
                Destroy(gameObject);
            }
        }
    }
}
