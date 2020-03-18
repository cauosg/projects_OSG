using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class spawnexplosion : MonoBehaviour {
    public Transform boss;
    public GameObject eventer;
    public float spawncooldown = 3.0f;
    public float fallingspd = 1.0f;
    public bool isspawn = false;
    public int maxspawn = 4;
    public float spawndistance = 2.0f;
   // public bool issample = false;
    private Vector3 bosspos;
    private Vector3 targetpos;
    private bool nowspawn = false;
    private int spawncount = 0;
    private float spawncooltime = 0.0f;
	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {

        if (!isspawn)
            return;
        if (spawncount > maxspawn)
        {
            spawncount = 0;
            spawncooltime = 0;
            isspawn = false;
            return;
        }
        if (!nowspawn) {
            spawncooltime = 0;
            spawncount++;
            nowspawn = true;
            bosspos = boss.position;
            
            targetpos = new Vector3(boss.position.x,0,9) + new Vector3(boss.forward.x,0,0).normalized * spawncount*spawndistance;
            
            transform.position = targetpos;

            Instantiate(eventer, targetpos, Quaternion.identity);
        }

        if (spawncooltime < spawncooldown)
            spawncooltime += Time.deltaTime;
        else
            nowspawn = false;

        //if (nowspawn)
        //{
        //    spawncooltime += Time.deltaTime;
        //}
    }
    public void setOn()
    {
        isspawn = true;
    }
}
